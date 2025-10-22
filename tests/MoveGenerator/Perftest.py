import time
from TestUtils import *

perft_tests = [
    ("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", # Start position
    { 
         1: 20,
         2: 400,
         3: 8902,
         4: 197281,
         5: 4865609,
         6: 119060324,
         7: 3195901860
        }),
    
    ("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", # Kiwipete
    { 
         1: 48,
         2: 2039,
         3: 97862,
         4: 4085603,
         5: 193690690,
         6: 8031647685
        }),

    ("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",
    { 
         1: 44,
         2: 1486,
         3: 62379,
         4: 2103487,
         5: 89941194
    }),

    ("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", # Node crunch
    { 
         1: 46,
         2: 2079,
         3: 89890,
         4: 3894594,
         5: 164075551,
         6: 6923051137
        }),

    ("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 
    {
         1: 6,
         2: 264,
         3: 9467,
         4: 422333,
         5: 15833292,
         6: 706045033
        }),

    ("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", # it always was enpassant...
    {
         1: 14,
         2: 191,
         3: 2812,
         4: 43238,
         5: 674624,
         6: 11030083
        })
]

def perft(engine, depth):
    nodes = 0
    start = time.time()
    engine.send(f"go perft {depth}")
    result_line = engine.read_until("Nodes searched:")
    end = time.time()

    nodes = int(result_line[15:])
    duration = end - start
    return nodes, duration

def run_perft_tests(engine_path):
    """
    Uruchamia zestaw testów perft.
    Zwraca:
        all_tests_passed (bool) - True, jeśli wszystkie testy się powiodły,
        report (str) - sformatowany raport z wynikami.
    """
    report_lines = []
    report_lines.append("-" * 80)
    report_lines.append("Move Generation Performence Test")
    report_lines.append("-" * 80)

    all_tests_passed = True
    total_nodes = 0
    total_time = 0
    
    with Engine(engine_path) as engine:

        for fen, expected_counts in perft_tests:
            report_lines.append(f"FEN: {fen}")


            engine.send(f"position fen {fen}")

            for depth, expected in expected_counts.items():

                nodes, duration = perft(engine, depth)
                nps = nodes / duration if duration > 0 else float('inf')

                line = f"Depth {depth}: nodes={nodes}, nps={nps:.0f} "
                if nodes == expected:
                    line += "PASSED"
                else:
                    line += f"FAILED (expected {expected})"
                    all_tests_passed = False

                report_lines.append(line)
                total_nodes += nodes
                total_time += duration
                if(not all_tests_passed):
                    break

            report_lines.append("-" * 80)
        avg_nps = total_nodes / total_time if total_time > 0 else 0
        report_lines.append(f"Average nps: {avg_nps:.0f}, Total time: {format_time(total_time)}")

    if all_tests_passed:
        report_lines.append("✅ PASSED")
    else:
        report_lines.append("❌ FAILED")
    report_lines.append("-" * 80)
    report = "\n".join(report_lines)

    return all_tests_passed, report
