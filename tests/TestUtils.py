import subprocess

RFERENCE_ENGINE = "/usr/games/stockfish"
BASELINE_ENGINE = "../.build-Baseline/CERCI-ChessEngine"

def format_time(seconds):
    hours = int(seconds // 3600)
    minutes = int((seconds % 3600) // 60)
    sec = seconds - 3600 * hours - 60 * minutes
    if(hours > 0):
        return f"{hours}h {minutes}min {sec:.4f}s"
    elif(minutes > 0):
        return f"{minutes}min {sec:.4f}s"
    else:
        return f"{sec:.6f}s"




class Engine:
    """
    Prosta klasa do komunikacji z silnikiem UCI.
    - działa na wszystkich systemach (Linux, Windows, macOS)
    - bez timeoutów - czyta, dopóki silnik coś wysyła
    """

    def __init__(self, path):
        self.path = path
        self.proc = subprocess.Popen(
            [path],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            bufsize=1, 
        )

    def send(self, command: str):
        if self.proc.stdin and self.proc.poll() is None:
            self.proc.stdin.write(command + "\n")
            self.proc.stdin.flush()

    # --- czytanie odpowiedzi ---
    def read_line(self):
        line = self.proc.stdout.readline()
        return line.strip()

    def read_until(self, pattern):
        while True:
            line = self.read_line()
            if line.startswith(pattern):
                return line

    # --- zarządzanie procesem ---
    def is_alive(self) -> bool:
        return self.proc.poll() is None

    def quit(self):
        try:
            if self.is_alive():
                self.send("quit")
                self.proc.wait(timeout=2)
        except Exception:
            self.terminate()

    def terminate(self):
        if self.is_alive():
            self.proc.terminate()

    # --- kontekst menedżera zasobów ---
    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.quit()
