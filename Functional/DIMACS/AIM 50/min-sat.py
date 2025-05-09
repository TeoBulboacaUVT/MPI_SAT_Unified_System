#!/usr/bin/env python3
"""
minisat_solver.py

A Python script using PySAT to solve a SAT problem specified in DIMACS CNF format
and report solving time in milliseconds.

Usage Examples:

1. Running from the script directory:
   ```bash
   cd path/to/script
   python minisat_solver.py \
       "C:/Users/Lenovo/CLionProjects/MPI_SAT_Unified/Functional/DIMACS/AIM 50/aim-50-1_6-no-1.cnf"
   ```

2. Invoking with full paths (Windows):
   ```bash
   python "C:/full/path/to/minisat_solver.py" \
       "C:/Users/Lenovo/.../aim-50-1_6-no-1.cnf"
   ```

3. Specifying a different solver:
   ```bash
   python minisat_solver.py --solver glucose3 example.cnf
   ```
"""
import sys
import argparse
import time
from pathlib import Path
from pysat.formula import CNF
from pysat.solvers import Solver


def solve_dimacs(path, solver_name='m22'):
    """
    Solve a CNF formula in DIMACS format using PySAT and the specified solver.

    :param path: path to the DIMACS CNF file
    :param solver_name: name of the underlying SAT solver (default: m22 for MiniSat22)
    """
    # Resolve and validate file path
    cnf_path = Path(path).expanduser()
    if not cnf_path.is_file():
        print(f"Error: DIMACS file not found at '{cnf_path}'", file=sys.stderr)
        sys.exit(1)

    # Read the CNF formula
    try:
        cnf = CNF(from_file=str(cnf_path))
    except Exception as e:
        print(f"Error reading DIMACS file: {e}", file=sys.stderr)
        sys.exit(1)

    with Solver(name=solver_name, bootstrap_with=cnf.clauses) as solver:
        start_time = time.perf_counter()
        is_sat = solver.solve()
        end_time = time.perf_counter()

        elapsed_ms = (end_time - start_time) * 1000

        if is_sat:
            model = solver.get_model()
            print("s SATISFIABLE")
            print("v {} 0".format(" ".join(str(lit) for lit in model)))
        else:
            print("s UNSATISFIABLE")

        print(format_time(elapsed_ms))


def format_time(ms):
    """
    Format the elapsed time in milliseconds.
    """
    return f"c Solving time: {ms:.2f} ms"


def main():
    parser = argparse.ArgumentParser(
        description='Solve a SAT problem in DIMACS format using PySAT and report time.'
    )
    parser.add_argument(
        'input', metavar='INPUT', help='Path to the input DIMACS CNF file'
    )
    parser.add_argument(
        '--solver', '-s', metavar='SOLVER', default='m22',
        help='Underlying SAT solver (e.g., m22, glucose3, cadical)'        
    )

    args = parser.parse_args()
    solve_dimacs(args.input, args.solver)


if __name__ == '__main__':
    main()
