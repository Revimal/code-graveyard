### Common definitions
```
(gdb) thread 2
[Switching to thread 2 (Thread 0x7fddad802700 (LWP 1311))]
#0  timer_get_prev_entries (time_val=2418294656249712, tim_lcore=6, prev=0x7fddad7ffc60) at /root/workspace/dpdk-stable/lib/librte_timer/rte_timer.c:224
224	 * Given a timer node in the skiplist, find the previous entries for it at
(gdb) l timer_get_prev_entries
211	{
212		unsigned lvl = priv_timer[tim_lcore].curr_skiplist_depth;
213		prev[lvl] = &priv_timer[tim_lcore].pending_head;
214		while(lvl != 0) {
215			lvl--;
216			prev[lvl] = prev[lvl+1];
217			while (prev[lvl]->sl_next[lvl] &&
218					prev[lvl]->sl_next[lvl]->expire <= time_val)
219				prev[lvl] = prev[lvl]->sl_next[lvl];
220		}
(gdb) p prev
$1 = (struct rte_timer **) 0x7fddad7ffc60
(gdb) p prev[lvl]
$2 = (struct rte_timer *) 0x14786f140
(gdb) p *prev[lvl]
$3 = {expire = 2417190772646698, sl_next = {0x1477e5240, 0x147843e00, 0x14786ce40, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}, status = {{state = 1, owner = 6}, u32 = 393217}, period = 0,
  f = 0x921ee0 <pz_timer_legacy_convfn>, arg = 0x14786f080}
```

We have a structure `struct rte_timer`; it is defined as like as the below:
```c
/**
 * A structure describing a timer in RTE.
 */
struct rte_timer
{
    uint64_t expire;       /**< Time when timer expire. */
    struct rte_timer *sl_next[MAX_SKIPLIST_DEPTH];
    volatile union rte_timer_status status; /**< Status of timer. */
    uint64_t period;       /**< Period of timer (0 if not periodic). */
    rte_timer_cb_t f;      /**< Callback function. */
    void *arg;             /**< Argument to callback function. */
};
```
The member named `sl_next` is an array of next pointers of the skip list, so we must iterate over `sl_next[depth]` if we want to check a loop exists. \
On the vanilla GDB, we must iterate pointers manually until a loop has been found. \
Of course, you have to manually check whether the output address has already been shown before with your own eyes! :P

### Without 'LoopChk'
```
(gdb) p prev[lvl]
$1 = (struct rte_timer *) 0x14786f140
(gdb) p prev[lvl]->sl_next[lvl]
$2 = (struct rte_timer *) 0x1477e5240
(gdb) p prev[lvl]->sl_next[lvl]->sl_next[lvl]
$3 = (struct rte_timer *) 0x147875a40
(gdb) p prev[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]
$4 = (struct rte_timer *) 0x14785e200
(gdb) p prev[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]
$5 = (struct rte_timer *) 0x14787a040
(gdb) p prev[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]
$6 = (struct rte_timer *) 0x14787a900
(gdb) p prev[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]
$7 = (struct rte_timer *) 0x14787c340
(gdb) p prev[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]
$8 = (struct rte_timer *) 0x14787f7c0
(gdb) p prev[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]
$9 = (struct rte_timer *) 0x147880940
..................................................
$27 = (struct rte_timer *) 0x147897000
(gdb) p prev[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]
$28 = (struct rte_timer *) 0x147891880
(gdb) p prev[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]
$29 = (struct rte_timer *) 0x14786ce40
(gdb) p prev[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]->sl_next[lvl]
$30 = (struct rte_timer *) 0x14786f140
```
Well... we found a loop at the end of 29 command inputs. \
Is this the best? Are you sure? \
Nope. There's an easier way to detect a linked list (or data structures derived from it) has a loop. \
Its the 'LoopChk'!

### With 'LoopChk'
```
(gdb) thread 2
[Switching to thread 2 (Thread 0x7fddad802700 (LWP 1311))]
#0  timer_get_prev_entries (time_val=2418294656249712, tim_lcore=6, prev=0x7fddad7ffc60) at /root/workspace/dpdk-stable/lib/librte_timer/rte_timer.c:224
224	 * Given a timer node in the skiplist, find the previous entries for it at
(gdb) source loopchk.py
(gdb) loopchk 64 prev[lvl] sl_next $lvl
Found looping with 28 nodes:
0: 0x14786f140
1: 0x1477e5240
2: 0x147875a40
3: 0x14785e200
4: 0x14787a040
5: 0x14787a900
6: 0x14787c340
7: 0x14787f7c0
8: 0x147880940
9: 0x147884f40
10: 0x14788c100
11: 0x14788ecc0
12: 0x147890700
13: 0x1477dfac0
14: 0x14787e640
15: 0x147882380
16: 0x147883500
17: 0x147883dc0
18: 0x147809e80
19: 0x147890fc0
20: 0x1478748c0
21: 0x14784b880
22: 0x147899300
23: 0x147875180
24: 0x1478a1f00
25: 0x1478167c0
26: 0x147897000
27: 0x147891880
28: 0x14786ce40
```
You can use it easily by importing a single Python source file from your GDB session!

* **The first argument: how many iterations allowed for finding a loop.** \
	If iteration count reaches limitation, the `loopchk` command will consider as there's no loop.

* **The second argument: expressions of base addresses that can be evaluated via GDB.** \
	If `print (expression)` command can be done in GDB, you can use `expression` directly for the second argument.

* **Variadic arguments (selective): access symbols separated by whitespace.** \
	If this argument is empty, the 'loopchk' command will consider as the access symbol is `next`. \
	The term 'access symbol' means 'the member name when accessing a structure / the index value when accessing an array / or the token name of any other accessible expression'. \
	With the prefix character `$`, the passed access symbol will be evaluated via GDB. \
	As an example, look at the above GDB session. \
	If we need to iterate the list by commands formed as `prev[lvl]->sl_next[lvl]` and its expansions, there are two access symbols: `sl_next` and `lvl`. \
	But the access symbol `lvl` is a variable that needs to evaluate, so we will append `$` prefix on it. \
	Therefore, the final variadic arguments will be like `sl_next $lvl`.

