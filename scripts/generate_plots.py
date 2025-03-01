import matplotlib.pyplot as plt
import numpy as np
import sys

def load_data(file_path, is_workspace=False):
    """
    Loads data from the specified file, parsing it into a 2D array of 0's and 1's
    or x, y coordinates for workspace.
    """
    print(f"Loading data from {file_path}...")

    with open(file_path, 'r') as file:
        data = file.read().strip().split('\n')

    if is_workspace:
        # For workspace, treat it just like the theta grid (0's and 1's)
        data_array = np.array([list(map(int, row)) for row in data])
        print(f"Workspace data loaded successfully. Total rows: {data_array.shape[0]}, Total columns: {data_array.shape[1]}")
        return data_array

    # For theta grid, also parse as 2D array of 0's and 1's
    data_array = np.array([list(map(int, row)) for row in data])
    print(f"Theta grid data loaded successfully. Total rows: {data_array.shape[0]}, Total columns: {data_array.shape[1]}")

    return data_array

def plot_data(theta_data, workspace_data):
    """
    Plots the theta grid data and workspace data side by side.
    """
    print("Plotting the data...")

    # Create a figure with 2 subplots, side by side
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 6))  # Adjust size for side-by-side

    # Plot the theta grid (0's and 1's)
    rows, cols = theta_data.shape
    x0, y0 = np.where(theta_data == 0)  # Coordinates for 0's
    x1, y1 = np.where(theta_data == 1)  # Coordinates for 1's

    ax1.scatter(y0, x0, color='blue', s=10, label='θ0')  # Scatter 0's (blue)
    ax1.scatter(y1, x1, color='red', s=10, label='θ1')   # Scatter 1's (red)
    ax1.set_xlabel('θ0')
    ax1.set_ylabel('θ1')
    ax1.set_title("Theta Grid")
    ax1.set_aspect('equal')
    ax1.legend()

    # Plot the workspace data (0's and 1's)
    x0_ws, y0_ws = np.where(workspace_data == 0)  # Coordinates for 0's in workspace
    x1_ws, y1_ws = np.where(workspace_data == 1)  # Coordinates for 1's in workspace

    ax2.scatter(y0_ws, x0_ws, color='blue', s=10, label='Clear')  # Scatter 0's (blue) for workspace
    ax2.scatter(y1_ws, x1_ws, color='red', s=10, label='Occupied')   # Scatter 1's (red) for workspace
    ax2.set_xlabel('X')
    ax2.set_ylabel('Y')
    ax2.set_title("Workspace")
    ax2.set_aspect('equal')
    ax2.legend()

    # Display the plots
    print("Displaying the plots...")
    plt.tight_layout()  # Ensures the plots fit well side by side
    plt.show()

def main():
    """
    Main function to load data and plot it.
    """
    # Check for input file path arguments
    if len(sys.argv) > 2:
        theta_file_path = sys.argv[1]
        workspace_file_path = sys.argv[2]
    else:
        # Default to input.txt and workspace.txt if no arguments are provided
        theta_file_path = 'cspace.txt'
        workspace_file_path = 'workspace.txt'

    print(f"Using theta file: {theta_file_path}")
    print(f"Using workspace file: {workspace_file_path}")

    # Load the input data from the files
    theta_data = load_data(theta_file_path)
    workspace_data = load_data(workspace_file_path, is_workspace=True)

    # Plot the data on a side-by-side comparison
    plot_data(theta_data, workspace_data)

if __name__ == '__main__':
    main()
