from http import HTTPStatus

# next imports are in conftest.py
# import pytest
# from fastapi.testclient import TestClient
# from fast_zero.app import app


# @pytest.fixture
# def client():
#     return TestClient(app)

# Original imports
# from fastapi.testclient import TestClient
# from fast_zero.app import app


# def test_root_deve_retornar_ok_e_ola_mundo():
def test_root_deve_retornar_ok_e_ola_mundo(client):
    # client = TestClient(app)  # Arrange (organização)

    response = client.get('/')  # Act (Ação)

    assert response.status_code == HTTPStatus.OK  # Assert
    assert response.json() == {'message': 'Olá Mundo!'}


# def test_exercicio_ola_mundo_em_html():
def test_exercicio_ola_mundo_em_html(client):
    # client = TestClient(app)

    response = client.get('/exercicio-html/')

    assert response.status_code == HTTPStatus.OK
    assert '<h1> Olá Mundo </h1>' in response.text


# def test_create_user():
def test_create_user(client):
    # client = TestClient(app)

    response = client.post(
        '/users/',
        json={
            'username': 'alice',
            'email': 'alice@example.com',
            'password': 'secret',
        },
    )
    # voltou status code correto?
    assert response.status_code == HTTPStatus.CREATED
    # validar UserSchema
    assert response.json() == {
        'username': 'alice',
        'email': 'alice@example.com',
        'id': 1,
    }


def test_read_users(client):
    response = client.get('/users/')
    assert response.status_code == HTTPStatus.OK
    assert response.json() == {
        'users': [
            {
                'username': 'alice',
                'email': 'alice@example.com',
                'id': 1,
            }
        ]
    }


def test_update_user(client):
    response = client.put(
        '/users/2',
        json={
            'password': '123',
            'username': 'testusername2',
            'email': 'test@test.com',
            'id': 1,
        },
    )
    assert response.json() == {
        'username': 'testusername2',
        'email': 'test@test.com',
        'id': 1,
    }


def test_delete_user(client):
    response = client.delete('/users/1')

    assert response.status_code == HTTPStatus.OK
    assert response.json() == {'message': 'User deleted'}
