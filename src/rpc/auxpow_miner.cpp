// Copyright (c) 2018-2024 Daniel Kraft
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <rpc/auxpow_miner.h>

#include <arith_uint256.h>
#include <auxpow.h>
#include <chainparams.h>
#include <consensus/merkle.h>
#include <net.h>
#include <node/context.h>
#include <rpc/blockchain.h>
#include <rpc/protocol.h>
#include <rpc/request.h>
#include <rpc/server_util.h>
#include <util/strencodings.h>
#include <util/time.h>
#include <validation.h>

#include <cassert>

namespace
{

using interfaces::Mining;

void auxMiningCheck(const node::NodeContext& node)
{
  const auto& connman = EnsureConnman (node);
  const auto& chainman = EnsureChainman (node);

  if (connman.GetNodeCount (ConnectionDirection::Both) == 0
        && !Params ().MineBlocksOnDemand ())
    throw JSONRPCError (RPC_CLIENT_NOT_CONNECTED,
                        "Bitcoinsilver is not connected!");

  if (chainman.IsInitialBlockDownload () && !Params ().MineBlocksOnDemand ())
    throw JSONRPCError (RPC_CLIENT_IN_INITIAL_DOWNLOAD,
                        "Bitcoinsilver is downloading blocks...");

  /* This should never fail, since the chain is already
     past the point of merge-mining start.  Check nevertheless.  */
  {
    LOCK (cs_main);
    const auto auxpowStart = Params ().GetConsensus ().nAuxpowStartHeight;
    if (chainman.ActiveHeight () + 1 < auxpowStart)
      throw std::runtime_error ("mining auxblock method is not yet available");
  }
}

}  // anonymous namespace

const CBlock*
AuxpowMiner::getCurrentBlock (ChainstateManager& chainman, Mining& miner,
                              const CTxMemPool& mempool,
                              const CScript& scriptPubKey, uint256& target)
{
  AssertLockHeld (cs);
  const CBlock* pblockCur = nullptr;

  {
    LOCK (cs_main);
    CScriptID scriptID (scriptPubKey);
    auto iter = curBlocks.find(scriptID);
    if (iter != curBlocks.end())
      pblockCur = iter->second;

    if (pblockCur == nullptr
        || pindexPrev != chainman.ActiveChain ().Tip ()
        || (mempool.GetTransactionsUpdated () != txUpdatedLast
            && GetTime () - startTime > 60))
      {
        if (pindexPrev != chainman.ActiveChain ().Tip ())
          {
            /* Clear old blocks since they're obsolete now.  */
            blocks.clear ();
            mapBlocks.clear ();
            curBlocks.clear ();
          }

        /* Create new block with nonce = 0 and extraNonce = 1.  */
        node::BlockCreateOptions opt;
        opt.coinbase_output_script = scriptPubKey;
        std::unique_ptr<interfaces::BlockTemplate> newTemplate
            = miner.createNewBlock (opt);
        if (newTemplate == nullptr)
          throw JSONRPCError (RPC_OUT_OF_MEMORY, "out of memory");
        blocks.push_back (std::make_unique<CBlock> (newTemplate->getBlock ()));
        CBlock& newBlock = *blocks.back ();

        /* Update state only when CreateNewBlock succeeded.  */
        txUpdatedLast = mempool.GetTransactionsUpdated ();
        pindexPrev = chainman.ActiveTip ();
        startTime = GetTime ();

        /* Finalise it by setting the version and building the merkle root.  */
        newBlock.hashMerkleRoot = BlockMerkleRoot (newBlock);
        newBlock.SetAuxpowVersion (true);

        /* Save in our map of constructed blocks.  */
        pblockCur = &newBlock;
        curBlocks.emplace(scriptID, pblockCur);
        mapBlocks[pblockCur->GetHash ()] = pblockCur;
      }
  }

  /* At this point, pblockCur is always initialised:  If we make it here
     without creating a new block above, it means that, in particular,
     pindexPrev == ::ChainActive ().Tip().  But for that to happen, we must
     already have created a pblockCur in a previous call, as pindexPrev is
     initialised only when pblockCur is.  */
  assert (pblockCur);

  arith_uint256 arithTarget;
  bool fNegative, fOverflow;
  arithTarget.SetCompact (pblockCur->nBits, &fNegative, &fOverflow);
  if (fNegative || fOverflow || arithTarget == 0)
    throw std::runtime_error ("invalid difficulty bits in block");
  target = ArithToUint256 (arithTarget);

  return pblockCur;
}

const CBlock*
AuxpowMiner::lookupSavedBlock (const std::string& hashHex) const
{
  AssertLockHeld (cs);

  const auto hash = uint256::FromHex (hashHex);
  if (!hash)
    throw JSONRPCError (RPC_INVALID_PARAMETER, "invalid block hash hex");

  const auto iter = mapBlocks.find (*hash);
  if (iter == mapBlocks.end ())
    throw JSONRPCError (RPC_INVALID_PARAMETER, "block hash unknown");

  return iter->second;
}

UniValue
AuxpowMiner::createAuxBlock (const JSONRPCRequest& request,
                             const CScript& scriptPubKey)
{
  LOCK (cs);

  const auto& node = EnsureAnyNodeContext (request);
  auxMiningCheck (node);
  const auto& mempool = EnsureMemPool (node);
  auto& chainman = EnsureChainman (node);
  auto& mining = EnsureMining (node);

  uint256 target;
  const CBlock* pblock = getCurrentBlock (chainman, mining, mempool,
                                          scriptPubKey, target);

  UniValue result(UniValue::VOBJ);
  result.pushKV ("hash", pblock->GetHash ().GetHex ());
  result.pushKV ("chainid", pblock->GetChainId ());
  result.pushKV ("previousblockhash", pblock->hashPrevBlock.GetHex ());
  result.pushKV ("coinbasevalue",
                 static_cast<int64_t> (pblock->vtx[0]->vout[0].nValue));
  result.pushKV ("bits", strprintf ("%08x", pblock->nBits));
  result.pushKV ("height", static_cast<int64_t> (pindexPrev->nHeight + 1));
  result.pushKV ("_target", HexStr (target));

  return result;
}

bool
AuxpowMiner::submitAuxBlock (const JSONRPCRequest& request,
                             const std::string& hashHex,
                             const std::string& auxpowHex) const
{
  const auto& node = EnsureAnyNodeContext (request);
  auxMiningCheck (node);
  auto& chainman = EnsureChainman (node);

  std::shared_ptr<CBlock> shared_block;
  {
    LOCK (cs);
    const CBlock* pblock = lookupSavedBlock (hashHex);
    shared_block = std::make_shared<CBlock> (*pblock);
  }

  const std::vector<unsigned char> vchAuxPow = ParseHex (auxpowHex);
  DataStream ss(vchAuxPow);
  std::unique_ptr<CAuxPow> pow(new CAuxPow ());
  ss >> *pow;
  shared_block->SetAuxpow (std::move (pow));
  assert (shared_block->GetHash ().GetHex () == hashHex);

  return chainman.ProcessNewBlock (shared_block, true, true, nullptr);
}

AuxpowMiner&
AuxpowMiner::get ()
{
  static AuxpowMiner* instance = nullptr;
  static RecursiveMutex lock;

  LOCK (lock);
  if (instance == nullptr)
    instance = new AuxpowMiner ();

  return *instance;
}
