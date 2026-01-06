import time
from TestUtils import *

positions_tests_set = [
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
    "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
    "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",
    "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10", 
    "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", 
    "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", ]

def eval(engine, depth):
    nodes = 0
    start = time.time()
    engine.send(f"go depth {depth}")
    engine.read_until("bestmove")
    end = time.time()

    duration = end - start
    return duration

def run_perft_tests(engine_path):
    """
    Uruchamia zestaw testów prędkości przeszukań.
    Zwraca:
        report (str) - sformatowany raport z wynikami.
    """
    report_lines = []
    report_lines.append("-" * 80)
    report_lines.append("Scherch Performence Test")
    report_lines.append("-" * 80)

    total_time = 0
    
    with Engine(engine_path) as engine:
        for fen in positions_tests_set:
            report_lines.append(f"FEN: {fen}")
            engine.send(f"position fen {fen}")

            for depth in range(1,8):

                duration = eval(engine, depth)
                line = f"Depth {depth}: duration={format_time(duration)}"
                report_lines.append(line)
                total_time += duration

            report_lines.append("-" * 80)
        report_lines.append(f"Total time: {format_time(total_time)}")

    report_lines.append("-" * 80)
    report = "\n".join(report_lines)

    return report
