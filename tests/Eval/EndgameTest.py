import time
import chess
import chess.engine
from TestUtils import *

endings = [ "1R6/RK6/8/8/8/8/6k1/8 b - - 0 1",  #dwie wierze
            "Q7/1K6/8/8/8/8/6k1/8 b - - 0 1",	#hetman
            "K7/7Q/8/8/3k4/4p3/8/8 w - - 0 1",  #hetman kontra pion
            "R7/1K6/8/8/8/8/6k1/8 w - - 0 1",	#wierza
            "R7/1KP5/8/8/8/8/6k1/7r w - - 0 1", #wierza kontra pion
            "B7/BK6/8/8/8/8/6k1/8 b - - 0 1",   #dwa gońce
            "1N6/BK6/8/8/8/8/6k1/8 b - - 0 1",  #skoczek i goniec
            ]

def test(engine_path, board):

    engine = chess.engine.SimpleEngine.popen_uci(engine_path)
    engine_stockfish = chess.engine.SimpleEngine.popen_uci(RFERENCE_ENGINE)

    moves_count = 0
    while not board.is_game_over():
        if board.turn == chess.WHITE:
            moves_count += 1             
            result = engine.play(board, chess.engine.Limit(time=0.6))
        else:
            result = engine_stockfish.play(board, chess.engine.Limit(time=0.1))

        board.push(result.move)
        # print(board)
        # print()
    engine.quit()
    engine_stockfish.quit()
    return board.result() == "1-0", moves_count

def run_endgame_tests(engine_path):

    report_lines = []
    report_lines.append("-" * 80)
    report_lines.append("Typical Endings Test")
    report_lines.append("-" * 80)

    all_tests_passed = True
    start = time.time()
    for fen in endings:
        board = chess.Board(fen)
        report_lines.append(f"FEN: {fen}")

        passed, moves_count = test(engine_path, board)
        line = f"Moves used: {moves_count} "
        if passed:
            line += "PASSED"
        else:
            line += f"FAILED"
            all_tests_passed = False

        report_lines.append(line)
        report_lines.append("-" * 80)
    end = time.time()

    report_lines.append(f"Total time: {format_time(end - start)}")

    if all_tests_passed:
        report_lines.append("✅ PASSED")
    else:
        report_lines.append("❌ FAILED")
    report_lines.append("-" * 80)
    report = "\n".join(report_lines)

    return all_tests_passed, report
