import gdb

class LoopChkCmd(gdb.Command):
    def __init__(self):
        super(LoopChkCmd, self).__init__("loopchk", gdb.COMMAND_USER)

    def _walk_through(self, lim, ptr, itp="next"):
        idx = 0
        res = ""
        node = ptr
        null = True
        itpl = itp.split()
        limit = int(lim)
        while idx < limit:
            res += "\n%d: 0x%x" % (idx, node)
            for itpv in itpl:
                if itpv.startswith("$"):
                    itpv = gdb.parse_and_eval(itpv[1:])
                node = node[itpv]
            if node == 0 or node == ptr:
                if node == ptr:
                    null = False
                break;
            idx += 1
        if not null:
            res = ("Found looping with %d nodes:" % idx) + res
        else:
            res = ("Looping not found in %d iterations:" % limit) + res
        return res

    def complete(self, text, word):
        return gdb.COMPLETE_SYMBOL

    def invoke(self, args, from_tty):
        try:
            alst = args.split(' ', 2)
            node = gdb.parse_and_eval(alst[1])
            try:
                print(self._walk_through(alst[0], node, alst[2]))
            except IndexError:
                print(self._walk_through(alst[0], node))
        except:
            print("Usage: loopchk [iteration-limit] [base-pointer] [next-expression]\n" +
                    "\t[Expression guide]\n" +
                    "\t 1. Each symbol must be separated by whitespace.\n"
                    "\t 2. '$' prefix makes the symbol the evaluated access.\n")

LoopChkCmd()
