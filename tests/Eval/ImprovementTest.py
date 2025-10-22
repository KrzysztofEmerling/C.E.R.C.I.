import time
import chess
import chess.engine
import random

from TestUtils import *

def load_positions(n : int):
    file_to_load = "Eval/filtered_positions.txt"
    random.seed(42)
    with open(file_to_load, "r") as file:
        lines = file.readlines()

    chosen_indices = random.sample(range(len(lines)), min(n, len(lines)))
    return [lines[i].rstrip('\n') for i in chosen_indices]

positions = load_positions(500) #test dla 1000 gier (500 białymi i czarnymi)


def test(engine_path, board1, board2):

    engine = chess.engine.SimpleEngine.popen_uci(engine_path)
    engine_baseline = chess.engine.SimpleEngine.popen_uci(BASELINE_ENGINE)

    while not board1.is_game_over():
        if board1.turn == chess.WHITE:
            result = engine.play(board1, chess.engine.Limit(time=0.2))
        else:
            result = engine_baseline.play(board1, chess.engine.Limit(time=0.2))

        board1.push(result.move)
        # print(board)
        # print()
    engine.quit()
    engine_baseline.quit()

    #gra czarnymi
    engine = chess.engine.SimpleEngine.popen_uci(engine_path)
    engine_baseline = chess.engine.SimpleEngine.popen_uci(BASELINE_ENGINE)

    while not board2.is_game_over():
        if board2.turn == chess.BLACK:
            result = engine.play(board2, chess.engine.Limit(time=0.2))
        else:
            result = engine_baseline.play(board2, chess.engine.Limit(time=0.2))

        board2.push(result.move)
        # print(board)
        # print()
    engine.quit()
    engine_baseline.quit()
    return board1.result(), board2.result()

def run_improvement_tests(engine_path):

    report_lines = []
    report_lines.append("-" * 80)
    report_lines.append("Typical Endings Test")
    report_lines.append("-" * 80)

    win_as_white = 0
    win_as_black = 0
    draw = 0
    all_games = 0

    start = time.time()
    for position in positions:
        moves = position.split()[3:]
        board1 = chess.Board()
        board2 = chess.Board()
        for move in moves:
            board1.push_uci(move)
            board2.push_uci(move)
        report_lines.append(f"FEN: {board1.fen()}")

        wresult, bresult = test(engine_path, board1, board2)
        line = f"Playing White: {wresult} Playing Black: {bresult}"
        if(wresult == "1-0"):
            win_as_white += 1
        elif(wresult != "0-1"):
            draw += 1

        if(bresult == "0-1"):
            win_as_black += 1
        elif(bresult != "1-0"):
            draw += 1
        
        all_games += 2
        report_lines.append(line)
        report_lines.append("-" * 80)
    end = time.time()

    report_lines.append(f"Total time: {format_time(end - start)}")
    report_lines.append(f"Win as White: {win_as_white}, Win as Black: {win_as_black}, Draws: {draw}")

    loses = all_games - (win_as_white + win_as_black)
    report_lines.append(f"Total win count: {win_as_white + win_as_black}, Draws: {draw}, Total loses: {loses}")
    report_lines.append("-" * 80)
    report = "\n".join(report_lines)

    return report
