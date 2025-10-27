import sys

def get_occ_var(k, r, c, M, N):
    """
    Replicates the variable mapping from the C++ encoder to find the
    integer representing Occupied(k, r, c).
    """
    return 1 + k * (M * N) + r * N + c

def get_edge_var(k, r, c, direction, M, N, K):
    """
    Replicates the variable mapping from the C++ encoder to find the
    integer representing Edge(k, r, c, direction).
    """
    edge_var_base = K * M * N
    return edge_var_base + 1 + k * (M * N * 4) + r * (N * 4) + c * 4 + direction

def parse_city_file(filename):
    """Parses the .city file to extract grid dimensions and metro lines."""
    with open(filename, 'r') as f:
        lines = f.readlines()
    
    params = list(map(int, lines[1].strip().split()))
    N, M, K = params[0], params[1], params[2]

    metro_lines = []
    for i in range(K):
        line_data = list(map(int, lines[2 + i].strip().split()))
        metro_lines.append({
            'startX': line_data[0], 'startY': line_data[1],
            'endX': line_data[2], 'endY': line_data[3]
        })
        
    return N, M, K, metro_lines

def parse_sat_output(filename):
    """Parses the .satoutput file to find all true variables."""
    with open(filename, 'r') as f:
        lines = f.readlines()

    if lines[0].strip() != "SAT":
        return None

    true_vars = set(map(int, lines[1].strip().split()))
    return true_vars

def print_solution_info(N, M, K, metro_lines, true_vars):
    """Prints a textual summary of the solution."""
    if true_vars is None:
        print("The problem is UNSATISFIABLE. No solution information available.")
        return

    print("="*40)
    print("        METRO MAP SOLUTION DETAILS")
    print("="*40)

    for k in range(K):
        line = metro_lines[k]
        print(f"\n--- Metro Line {k} ---")
        print(f"Path from ({line['startX']},{line['startY']}) to ({line['endX']},{line['endY']})")
        print("\n  Occupied Cells:")
        
        found_occupied = False
        for r in range(M):
            for c in range(N):
                occ_var = get_occ_var(k, r, c, M, N)
                if occ_var in true_vars:
                    print(f"    - Cell ({r}, {c}) is occupied.")
                    found_occupied = True
        if not found_occupied:
            print("    (None)")


        print("\n  Activated Edges (Outgoing from cell):")
        found_edges = False
        # Directions: 0:Up, 1:Right, 2:Down, 3:Left
        directions = {0: "UP", 1: "RIGHT", 2: "DOWN", 3: "LEFT"}
        for r in range(M):
            for c in range(N):
                for direction_code, direction_name in directions.items():
                    edge_var = get_edge_var(k, r, c, direction_code, M, N, K)
                    if edge_var in true_vars:
                        print(f"    - From ({r}, {c}) -> {direction_name}")
                        found_edges = True
        if not found_edges:
            print("    (None)")
        print("-" * 40)


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python print_info.py <input.city> <input.satoutput>")
        sys.exit(1)

    city_file = sys.argv[1]
    sat_output_file = sys.argv[2]

    try:
        N, M, K, metro_lines = parse_city_file(city_file)
        true_vars = parse_sat_output(sat_output_file)
        
        print_solution_info(N, M, K, metro_lines, true_vars)

    except FileNotFoundError as e:
        print(f"Error: Could not open file {e.filename}")
        sys.exit(1)
    except Exception as e:
        print(f"An error occurred: {e}")
        sys.exit(1)