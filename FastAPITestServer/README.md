# Fast Zero

Fast Zero is a FastAPI-based project designed for managing user data and serving an ESP32 image dashboard. It includes endpoints for user management, image uploads, and real-time updates using Server-Sent Events (SSE).

## Features

- **User Management**: Create, read, update, and delete users.
- **ESP32 Image Dashboard**: Display grayscale, binary mask, and full-resolution images uploaded from an ESP32 device.
- **Real-Time Updates**: Automatically refresh images on the dashboard using SSE.
- **Static and Template Rendering**: Serve static files and render HTML templates using Jinja2.

## Project Structure

```
fast_zero/
├── fast_zero/__init__.py
├── fast_zero/app.py          # FastAPI application with user management and endpoints
├── fast_zero/schemas.py      # Pydantic models for request and response validation
tests/
├── tests/conftest.py         # Pytest fixtures for testing
├── tests/test_app.py         # Unit tests for the FastAPI application
templates/
├── templates/index.html      # HTML template for the ESP32 image dashboard
static/
├── static/styles.css         # CSS for styling the dashboard
├── static/update.js          # JavaScript for real-time image updates
├── static/convert_images.py  # Script for resizing and saving images
main.py                       # Main application for serving the ESP32 dashboard
pyproject.toml                # Project configuration and dependencies
poetry.lock                   # Poetry lock file
README.md                     # Project documentation
```

## Installation

1. **Clone the Repository**  
   Clone the repository to your local machine:
   ```sh
   git clone <repository-url>
   cd fast_zero
   ```

2. **Install Python**  
   Ensure you have Python 3.12 installed. You can check your Python version with:
   ```sh
   python --version
   ```

3. **Install Poetry**  
   Install [Poetry](https://python-poetry.org/) for dependency management:
   ```sh
   pip install poetry
   ```

4. **Install Dependencies**  
   Use Poetry to install the project dependencies:
   ```sh
   poetry install
   ```

5. **Activate the Virtual Environment**  
   Activate the virtual environment created by Poetry:
   ```sh
   poetry shell
   ```

6. **Prepare Static Files**  
   Ensure the `static/current` directory exists and contains placeholder images. You can use the `convert_images.py` script to generate resized images:
   ```sh
   python static/convert_images.py <path-to-input-image>
   ```

## Usage

### Running the FastAPI Application

To run the FastAPI application for user management:

```sh
poetry run task run
```

### Running the ESP32 Dashboard

To run the ESP32 image dashboard:

```sh
poetry run task runesp32
```

The application will be available at `http://127.0.0.1:8000`.

### Endpoints

#### User Management

- `GET /users/`: Retrieve all users.
- `POST /users/`: Create a new user.
- `PUT /users/{user_id}`: Update an existing user.
- `DELETE /users/{user_id}`: Delete a user.

#### ESP32 Dashboard

- `GET /`: Render the image dashboard.
- `POST /upload`: Upload images (grayscale, mask, and full-resolution).
- `GET /events`: SSE endpoint for real-time updates.

### Running Tests

Run the test suite using Pytest:

```sh
poetry run task test
```

### Linting and Formatting

Check code quality and apply formatting:

```sh
poetry run task lint
poetry run task format
```

## License

This project is licensed under the MIT License. See the LICENSE file for details.

## Author

Developed by Marcelmtb2 (<mtb2@softex.cin.ufpe.br>).