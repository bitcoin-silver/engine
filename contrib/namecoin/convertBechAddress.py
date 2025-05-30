#!/usr/bin/env python3

#   Convert Address - convert Bitcoin to Bitcoinsilver bech32 addresses
#   Copyright (C) 2018-2021  Daniel Kraft <d@domob.eu>
#
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU Affero General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Affero General Public License for more details.
#
#   You should have received a copy of the GNU Affero General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.

import sys

import segwit_addr

if len (sys.argv) not in [2, 3]:
  print ("Usage: convertBechAddress.py ADDRESS [TO-HRP]")
  sys.exit (-1);

addr = sys.argv[1]
if len (sys.argv) >= 3:
  hrp = sys.argv[2]
else:
  hrp = "nc"

enc, oldHrp, data = segwit_addr.bech32_decode (addr)
print ("Encoding %s, old HRP: %s" % (enc, oldHrp))
newAddr = segwit_addr.bech32_encode (enc, hrp, data)
print (newAddr)
