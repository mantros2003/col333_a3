import sys
import matplotlib.pyplot as plt
import matplotlib.colors as mcolors
import matplotlib.patches as mpatches

def get_occ_var(k, r, c, M, N):
    """
    Replicates the variable mapping from the C++ encoder to find the
    integer representing Occupied(k, r, c).
    This function MUST be kept in sync with the C++ encoder's mapping.
    """
    # In the C++ code: occ_var_base = 0;
    # variable = occ_var_base + 1 + k * (M * N) + r * N + c;
    return 1 + k * (M * N) + r * N + c

def parse_city_file(filename):
    """Parses the .city file to extract grid dimensions and metro lines."""
    with open(filename, 'r') as f:
        lines = f.readlines()

    scenario = int(lines[0].strip())
    
    params = list(map(int, lines[1].strip().split()))
    N, M, K, J = params[0], params[1], params[2], params[3]

    metro_lines = []
    for i in range(K):
        line_data = list(map(int, lines[2 + i].strip().split()))
        metro_lines.append({
            'startX': line_data[0], 'startY': line_data[1],
            'endX': line_data[2], 'endY': line_data[3]
        })
        
    return N, M, K, J, metro_lines

def parse_sat_output(filename):
    """Parses the .satoutput file to find all true variables."""
    with open(filename, 'r') as f:
        lines = f.readlines()

    if lines[0].strip() != "SAT":
        return None

    true_vars = set(map(int, lines[1].strip().split()))
    return true_vars

def visualize_grid(N, M, K, metro_lines, true_vars):
    """Creates and displays a visualization of the metro map."""
    if true_vars is None:
        print("The problem is UNSATISFIABLE. No map to render.")
        return

    # Create a grid to store which line occupies which cell
    # 0 means empty, k+1 means occupied by line k
    grid = [[0 for _ in range(N)] for _ in range(M)]

    for k in range(K):
        for r in range(M):
            for c in range(N):
                occ_var = get_occ_var(k, r, c, M, N)
                if occ_var in true_vars:
                    grid[r][c] = k + 1

    # --- Plotting ---
    fig, ax = plt.subplots(figsize=(N/2, M/2))
    
    # Define colors: one for each line + a background color
    tab10_colors = plt.colormaps['tab10'].resampled(K)(range(K))
    colors = ['#FFFFFF'] + [mcolors.to_hex(c) for c in tab10_colors]
    cmap = mcolors.ListedColormap(colors)
    
    # Display the grid as an image
    ax.imshow(grid, cmap=cmap, origin='upper', extent=[-0.5, N - 0.5, M - 0.5, -0.5])

    # Mark start and end points for each metro line
    for k, line in enumerate(metro_lines):
        # Start point
        ax.text(line['startX'], line['startY'], f'S{k}', 
                color='black', ha='center', va='center', weight='bold',
                bbox=dict(boxstyle='circle,pad=0.3', fc='white', ec='black'))
        # End point
        ax.text(line['endX'], line['endY'], f'E{k}', 
                color='black', ha='center', va='center', weight='bold',
                bbox=dict(boxstyle='circle,pad=0.3', fc='white', ec='black'))

    # Configure grid lines and ticks
    ax.set_xticks(range(N))
    ax.set_yticks(range(M))
    ax.set_xticklabels(range(N))
    ax.set_yticklabels(range(M))
    ax.set_xticks([x - 0.5 for x in range(N)], minor=True)
    ax.set_yticks([y - 0.5 for y in range(M)], minor=True)
    ax.grid(which='minor', color='black', linestyle='-', linewidth=2)
    ax.tick_params(which='minor', size=0)
    
    # Invert y-axis to have (0,0) at the top-left
    ax.invert_yaxis()

    # Create a legend
    legend_patches = [mpatches.Patch(color=colors[k+1], label=f'Line {k}') for k in range(K)]
    ax.legend(handles=legend_patches, bbox_to_anchor=(1.05, 1), loc='upper left')

    ax.set_title('Metro Map Visualization', fontsize=16)
    plt.tight_layout()
    plt.show()


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python visualize.py <input.city> <input.satoutput>")
        sys.exit(1)

    city_file = sys.argv[1]
    sat_output_file = sys.argv[2]

    try:
        N, M, K, J, metro_lines = parse_city_file(city_file)
        true_vars = parse_sat_output(sat_output_file)
        
        visualize_grid(N, M, K, metro_lines, true_vars)

    except FileNotFoundError as e:
        print(f"Error: Could not open file {e.filename}")
        sys.exit(1)
    except Exception as e:
        print(f"An error occurred: {e}")
        sys.exit(1)