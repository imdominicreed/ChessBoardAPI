#!/usr/bin/env python3

def run_perft(program, position, depth):
    from subprocess import Popen, PIPE, STDOUT
    import re
    try:
        p = Popen([program], stdout=PIPE, stdin=PIPE, stderr=PIPE, text=True)
        stdout_data, stderr = p.communicate(input=f"position {position}\ngo perft {depth}\n")
        paths_tuple = re.findall('^(\w+): (\d+)$', stdout_data, flags=re.MULTILINE)
        return {mov: int(cnt) for [mov, cnt] in paths_tuple}
    except:
        print("STDERROR")
        print(f"position {position}\ngo perft {depth}\n")
        exit()
    

def diff_perft(program1, program2, position, depth):
    chess_engine_perft = run_perft(program1, position, depth)
    stock_perft = run_perft(program2, position, depth)

    return {
        "extra": [move for move in chess_engine_perft if move not in stock_perft],
        "missing": [move for move in stock_perft if move not in chess_engine_perft],
        "mismatched": [[move, stock_perft[move], chess_engine_perft[move]] for move in chess_engine_perft if move in stock_perft and stock_perft[move] != chess_engine_perft[move]],
    }


def search_for_bug(program1, program2, position, depth):
    results = diff_perft(program1, program2, position, depth)
    if len(results["extra"]) > 0:
        print(f"position {position}\ngo perft {depth}\n")
        print("extras from", position, results["extra"])
        return False

    if len(results["missing"]) > 0:
        print(f"position {position}\ngo perft {depth}\n")
        print("missing from", position, results["missing"])
        return False
    if len(results["mismatched"]) > 0:
        [move, _, _] = results["mismatched"][0]
        return search_for_bug(program1, program2, f"{position} {move}", depth-1)
    return True
f = open("perftsuite.epd", "r")
lines = f.readlines()
for l in lines:
    fen = l.split(';')[0]
    print(fen)
    if not search_for_bug('build/chess','stockfish', 'fen ' + fen + ' moves ', 5):
        break
print('PASSED ALL TESTS!')
