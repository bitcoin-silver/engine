// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2021 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_SCRIPT_BITCOINSILVERCONSENSUS_H
#define BITCOIN_SCRIPT_BITCOINSILVERCONSENSUS_H

#include <stdint.h>

#if defined(BUILD_BITCOIN_INTERNAL) && defined(HAVE_CONFIG_H)
#include <config/bitcoin-config.h>
  #if defined(_WIN32)
    #if defined(HAVE_DLLEXPORT_ATTRIBUTE)
      #define EXPORT_SYMBOL __declspec(dllexport)
    #else
      #define EXPORT_SYMBOL
    #endif
  #elif defined(HAVE_DEFAULT_VISIBILITY_ATTRIBUTE)
    #define EXPORT_SYMBOL __attribute__ ((visibility ("default")))
  #endif
#elif defined(MSC_VER) && !defined(STATIC_LIBBITCOINCONSENSUS)
  #define EXPORT_SYMBOL __declspec(dllimport)
#endif

#ifndef EXPORT_SYMBOL
  #define EXPORT_SYMBOL
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define BITCOINSILVERCONSENSUS_API_VER 2

typedef enum bitcoinsilverconsensus_error_t
{
    bitcoinsilverconsensus_ERR_OK = 0,
    bitcoinsilverconsensus_ERR_TX_INDEX,
    bitcoinsilverconsensus_ERR_TX_SIZE_MISMATCH,
    bitcoinsilverconsensus_ERR_TX_DESERIALIZE,
    bitcoinsilverconsensus_ERR_AMOUNT_REQUIRED,
    bitcoinsilverconsensus_ERR_INVALID_FLAGS,
    bitcoinsilverconsensus_ERR_SPENT_OUTPUTS_REQUIRED,
    bitcoinsilverconsensus_ERR_SPENT_OUTPUTS_MISMATCH
} bitcoinsilverconsensus_error;

/** Script verification flags */
enum
{
    bitcoinsilverconsensus_SCRIPT_FLAGS_VERIFY_NONE                = 0,
    bitcoinsilverconsensus_SCRIPT_FLAGS_VERIFY_P2SH                = (1U << 0), // evaluate P2SH (BIP16) subscripts
    bitcoinsilverconsensus_SCRIPT_FLAGS_VERIFY_DERSIG              = (1U << 2), // enforce strict DER (BIP66) compliance
    bitcoinsilverconsensus_SCRIPT_FLAGS_VERIFY_NULLDUMMY           = (1U << 4), // enforce NULLDUMMY (BIP147)
    bitcoinsilverconsensus_SCRIPT_FLAGS_VERIFY_CHECKLOCKTIMEVERIFY = (1U << 9), // enable CHECKLOCKTIMEVERIFY (BIP65)
    bitcoinsilverconsensus_SCRIPT_FLAGS_VERIFY_CHECKSEQUENCEVERIFY = (1U << 10), // enable CHECKSEQUENCEVERIFY (BIP112)
    bitcoinsilverconsensus_SCRIPT_FLAGS_VERIFY_WITNESS             = (1U << 11), // enable WITNESS (BIP141)
    bitcoinsilverconsensus_SCRIPT_FLAGS_VERIFY_TAPROOT             = (1U << 17), // enable TAPROOT (BIPs 341 & 342)
    bitcoinsilverconsensus_SCRIPT_FLAGS_VERIFY_ALL                 = bitcoinsilverconsensus_SCRIPT_FLAGS_VERIFY_P2SH | bitcoinsilverconsensus_SCRIPT_FLAGS_VERIFY_DERSIG |
                                                               bitcoinsilverconsensus_SCRIPT_FLAGS_VERIFY_NULLDUMMY | bitcoinsilverconsensus_SCRIPT_FLAGS_VERIFY_CHECKLOCKTIMEVERIFY |
                                                               bitcoinsilverconsensus_SCRIPT_FLAGS_VERIFY_CHECKSEQUENCEVERIFY | bitcoinsilverconsensus_SCRIPT_FLAGS_VERIFY_WITNESS |
                                                               bitcoinsilverconsensus_SCRIPT_FLAGS_VERIFY_TAPROOT
};

typedef struct {
    const unsigned char *scriptPubKey;
    unsigned int scriptPubKeySize;
    int64_t value;
} UTXO;

/// Returns 1 if the input nIn of the serialized transaction pointed to by
/// txTo correctly spends the scriptPubKey pointed to by scriptPubKey under
/// the additional constraints specified by flags.
/// If not nullptr, err will contain an error/success code for the operation
EXPORT_SYMBOL int bitcoinsilverconsensus_verify_script(const unsigned char *scriptPubKey, unsigned int scriptPubKeyLen,
                                                 const unsigned char *txTo        , unsigned int txToLen,
                                                 unsigned int nIn, unsigned int flags, bitcoinsilverconsensus_error* err);

EXPORT_SYMBOL int bitcoinsilverconsensus_verify_script_with_amount(const unsigned char *scriptPubKey, unsigned int scriptPubKeyLen, int64_t amount,
                                    const unsigned char *txTo        , unsigned int txToLen,
                                    unsigned int nIn, unsigned int flags, bitcoinsilverconsensus_error* err);

EXPORT_SYMBOL int bitcoinsilverconsensus_verify_script_with_spent_outputs(const unsigned char *scriptPubKey, unsigned int scriptPubKeyLen, int64_t amount,
                                    const unsigned char *txTo        , unsigned int txToLen,
                                    const UTXO *spentOutputs, unsigned int spentOutputsLen,
                                    unsigned int nIn, unsigned int flags, bitcoinsilverconsensus_error* err);

EXPORT_SYMBOL unsigned int bitcoinsilverconsensus_version();

#ifdef __cplusplus
} // extern "C"
#endif

#undef EXPORT_SYMBOL

#endif // BITCOIN_SCRIPT_BITCOINSILVERCONSENSUS_H
