Oh, well I'll use this file to take some notes about things mostly regarding 
bitcoin and the bitcoin node interaction that I might not explicitly have in 
my software. Yea, bits an pieces, not necessarily in order that may be of 
use later.

Interacting witht the node via GRPC is simple using the bitcoin-cli which
offers among other things a client for the bitcoind. Although there are things
I will calculate with code in bulldozer, there is a bunch of things that is 
easier to get from the node server.

## Getting the block template
For instance, this is unavoidable, I will need to get the next block to sign.
So for that I'm using this command:

```bash
bitcoin-cli getblocktemplate  '{"rules": ["segwit"]}' 
```

or the following to tell the client to use a test network instead (all the 
following snippets will be using a testnetwork).

```bash
bitcoin-cli -testnet4 getblocktemplate  '{"rules": ["segwit"]}' 
```

This will return a json object with the block template, including all the 
things I need calculate the merkle root, create a coinbase transaction, 
and form a block header; i.e. this is previous block hash, the 
difficulty (bits), the height, and the coinbase reward including the
both subsidy and the fees.

The result in addition to provide all the former, it also comes with a `longpollid`
which is a string that I can use to run the RPC call and had it blocked
until the next block is ready to be mined. This is useful to avoid polling
the server every second for a new block template.
In my case this is good because if this happens before I find a PoW, I can
stop the computation and try out with the next block instead.

An example of how this works is the following:

```bash
bitcoin-cli -testnet4 getblocktemplate  '{"longpollid": "00000000e907efe304bc0ab0e37c753a64b3d569af242fa7c00ac50f33be13c173301", "rules": ["segwit"]}'
```

## Creating a wallet and an address
This will be handy for when I want to use the RPC calls to create a rawtransaction. 
Certainly this may not be required when I make my program (just the public address
will be required in that case I believe). 
    
```bash
bitcoin-cli -testnet4 createwallet "testwallet"
bitcoin-cli  -testnet4 getnewaddress "testwallet" "p2sh-segwit"
```

Among the options for getnewaddress are `legacy`, `p2sh-segwit`, and `bech32`.

## Creating a raw transaction
The only transaction I plan to create over and over is the coinbase transaction.
I know how to generate it and build the structure for it, but I thought it could be 
handy to have a way to check against it. 
To create one with the CLI I should use this command:
```bash
bitcoin-cli -testnet4 createrawtransaction \
    '[{"txid":"0000000000000000000000000000000000000000000000000000000000000000","vout":2147483647}]' \
    '[{"2MtQpkFGr8jwcncbNvdMGTBJ3sGHiTyEja5":30000}]'
```
Where the address has to be an address that's available in the client's wallet.
In this case the address `2MtQpkFGr8jwcncbNvdMGTBJ3sGHiTyEja5` is the address
I created in the previous step.
Notice that the main two arguments tells createtransaction that we're asking 
for the coinbase transaction to be created.

The result of this transaction is a hex string that follows the structure of a
transaction, however each of the fields bytes are reversed (this is what some people
call the RPC format), this is, the bytes of each field are in big-endian.
If I want to see what's in this transaction I can use the following command:

```bash
bitcoin-cli -testnet4 decoderawtransaction 02000000010000000000000000000000000000000000000000000000000000000000000000ffffff7f00fdffffff010030ef7dba02000017a9140cc8e24a0714fa7a1d261e1c3d21a68fa617920c8700000000
```

Which should show something like this:
```bash
{
  "txid": "297ec235ae847d0f85a6678ee178297b1a604a2898a2b9daf47462546498e6bf",
  "hash": "297ec235ae847d0f85a6678ee178297b1a604a2898a2b9daf47462546498e6bf",
  "version": 2,
  "size": 83,
  "vsize": 83,
  "weight": 332,
  "locktime": 0,
  "vin": [
    {
      "txid": "0000000000000000000000000000000000000000000000000000000000000000",
      "vout": 2147483647,
      "scriptSig": {
        "asm": "",
        "hex": ""
      },
      "sequence": 4294967293
    }
  ],
  "vout": [
    {
      "value": 30000.00000000,
      "n": 0,
      "scriptPubKey": {
        "asm": "OP_HASH160 0cc8e24a0714fa7a1d261e1c3d21a68fa617920c OP_EQUAL",
        "desc": "addr(2MtQpkFGr8jwcncbNvdMGTBJ3sGHiTyEja5)#tk7pk3fl",
        "hex": "a9140cc8e24a0714fa7a1d261e1c3d21a68fa617920c87",
        "address": "2MtQpkFGr8jwcncbNvdMGTBJ3sGHiTyEja5",
        "type": "scripthash"
      }
    }
  ]
}
```

This is interesting as the txid field can be used to compare against the results I 
may be able to generate from my software (bts, this hexstring entirely, will be in 
big-endian too). 


## Running the bitcoin node
Well this is straightforward. Worth mentioning that running the server will take a
while the first times (even if you use pruning), as it has to download the whole
blockchain anyway. And well the whole blockchain is about 500GB and blocks are downloaded
one by one, and checked (it seems) one by one. Is not a slow process (less than a few 
seconds) but over time adds up making it time consumming.
Recomendations is to run this with really good internet and with something around 1T
or more of free space.

```bash
bitcoind -testnet4
```

Each chain the server downloads has a different placement in the filesystem (`~./bitcoin/<chain>/*`), so 
you can download the testnet4, regtest, and mainet and have one server for each of them.

Last thing in regard to this us using the prune option (this can be set 
in the configfile as well), which will tell the server to work with a 
limitted amount of space to store the last X GBs of the blockchain.
As mentioned earlier, don't expect this to speed up the process of downloading,
as it will still download the whole blockchain, from the start even if 
you're not going to keep those blocks.

## Running the UI (bitcoin-qt)
I haven't been successful with this yet, mostly because it seems I can't run
the UI with the server running, as the GUI itself has a server.

Worth noting tho, that you can also if you so desire to, use the -prune=550
as well when running the GUI:
```bash
bitcoin-qt -testnet4 -prune=550
```