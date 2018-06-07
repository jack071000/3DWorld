#include "stdafx.h"
#include "Player1.h"


Player1::Player1()
{
}

Player1::~Player1()
{
}

void Player1::Init()
{
	m_Mesh = Mesh::Create("./Resource/Mesh/", "TombStone.x");


	AddChild(m_Mesh);
}

void Player1::Update()
{
	GameObject::Update();

	printf("플레이어의 Rot Y : %f\n", m_RotY);

	if (GetAsyncKeyState('A') & 0x8000)
		m_RotY -= 0.1f;
	else if (GetAsyncKeyState('D') & 0x8000)
		m_RotY += 0.1f;
	if (GetAsyncKeyState('W') & 0x8000)
		m_Position.z += 0.1f;
	else if (GetAsyncKeyState('S') & 0x8000)
		m_Position.z -= 0.1f;
	if (GetAsyncKeyState('Q') & 0x8000)
		m_Position.y -= 0.1f;
	else if (GetAsyncKeyState('E') & 0x8000)
		m_Position.y += 0.1f;

}

void Player1::Render()
{
	GameObject::Render();
}
