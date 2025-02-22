# Bulldozer

Bulldozer is a bitcoin miner that uses a mix of CPU and GPU compute to
calculate the PoW.

<!-- image of a bulldozer -->
![Bulldozer](docs/imgs/bulldozer.jpg)

I thought initially to call this project "digger", but thought that bulldozer
sounded more intimidating, powerful, and reflective of the type of program 
I want to build: I'll use CUDA algorithms as a big wide blade to clear up the
solution space in the search for the nonce.

## Motivation

I've build in the past a quick implementation of bitcoin for CUDA, trying to 
test some wacky idea. I have experiencing implementing SHA-256, and running
benchmarks on it, and I'm very queen to find what's the most performant SHA-256
out there, and get my implementation to be as close as possible to that or better
in order to compete with modern miners at the same level.

So far there is still work left in terms of the optimizations on my SHA-256 
implementation, and despite I have thought a few alternatives, in the end, the 
best is to have a specific field and context where to fine tune the implementations 
to get a great performance.

## Hypothesis

These are the questions I want to answer with this project:

- Can I calculate the merkle root on a CPU for an average block (of 2000 txs) in less than 1 second?
  ANSWER: yes. As of now I see that 2500 addresses can be calculated 0.000689 seconds, 
          that's more than 1200 merkle roots per second. We could improve it, but it's a good start.
- Can I calculate a double SHA-256 on a GPU device in less than 4 ns?
- Can I actually manage to calculate 2^32 SHA-256d hashes in 17 seconds or less?

## Approach
The only way to find out is starting out. And I'll optimize as I go.

I'll use libwolfssl to the CPU implementation as I can calculate a double hash in 
about 400 ns. Not the best, but is a start. 
The idea then is to use this to calculate the merkle root of a given input.
Once I have that, I need to start with the algorithm of the main program, which will
launch a kernel to calculate in parallel double SHA-256 hashes for 2^32 headers + nonces.

I believe this implementation should take 1 to 2 days.
If the result is as expected, without optimizations, it would be good to got, and start building up 
the pieces so the miner connects to the network and does its thing. Let's say that takes another 
couple of days. 
   