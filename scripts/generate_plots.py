import matplotlib.pyplot as plt
import numpy as np


def load_data(file_path):
    """
    Loads data from a single file, extracting CSPACE and WSPACE sections.
    """
    print(f"Loading data from {file_path}...")

    with open(file_path, 'r') as file:
        content = file.read()

    # Extract sections
    cspace_section = extract_section(content, "CSPACE")
    wspace_section = extract_section(content, "WSPACE")

    if cspace_section is None or wspace_section is None:
        raise ValueError("Could not find both CSPACE and WSPACE sections in the file.")

    # Convert sections to numpy arrays
    cspace_data = parse_grid(cspace_section)
    wspace_data = parse_grid(wspace_section)

    print("Data loaded successfully.")
    print(f"CSPACE - Rows: {cspace_data.shape[0]}, Columns: {cspace_data.shape[1]}")
    print(f"WSPACE - Rows: {wspace_data.shape[0]}, Columns: {wspace_data.shape[1]}")

    return cspace_data, wspace_data


def extract_section(content, section_name):
    """Extracts data between markers ===SECTION=== in the given text."""
    marker = f"==={section_name}==="
    sections = content.split(marker)

    if len(sections) < 3:
        return None  # Section markers not found properly

    return sections[1].strip()


def parse_grid(section):
    """Converts a string section of 0's and 1's into a numpy array."""
    return np.array([list(map(int, row)) for row in section.split('\n')])


def plot_data(cspace_data, wspace_data):
    """
    Plots the cspace and workspace data side by side.
    """
    print("Plotting the data...")

    # Create a figure with 2 subplots, side by side
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 6))

    # Plot the CSPACE data
    x0, y0 = np.where(cspace_data == 0)
    x1, y1 = np.where(cspace_data == 1)
    ax1.scatter(y0, x0, color='blue', s=10, label='θ0')
    ax1.scatter(y1, x1, color='red', s=10, label='θ1')
    ax1.set_xlabel('θ0')
    ax1.set_ylabel('θ1')
    ax1.set_title("CSPACE")
    ax1.set_aspect('equal')
    ax1.legend()

    # Plot the WSPACE data
    x0_ws, y0_ws = np.where(wspace_data == 0)
    x1_ws, y1_ws = np.where(wspace_data == 1)
    ax2.scatter(y0_ws, x0_ws, color='blue', s=10, label='Clear')
    ax2.scatter(y1_ws, x1_ws, color='red', s=10, label='Occupied')
    ax2.set_xlabel('X')
    ax2.set_ylabel('Y')
    ax2.set_title("WSPACE")
    ax2.set_aspect('equal')
    ax2.legend()

    # Display the plots
    print("Displaying the plots...")
    plt.tight_layout()
    plt.show()


def main():
    """
    Main function to load data and plot it.
    """
    file_path = "pathfind.txt"
    print(f"Using file: {file_path}")

    # Load the input data from the file
    cspace_data, wspace_data = load_data(file_path)

    # Plot the data
    plot_data(cspace_data, wspace_data)


if __name__ == '__main__':
    main()
