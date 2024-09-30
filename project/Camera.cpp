#include "Camera.h"

Camera::Camera()
{
}

void Camera::Update()
{
	//TransformからworldMatrixを作る
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	//cameraMatrixからviewMatrixを作る
	Matrix4x4 viewMatrix = Inverse(worldMatrix);
	//プロジェクション行列更新
	projectionMatrix = MakePerspectiveFovMatrix(horizontalViewingAngle, aspect, nearClip, farClip);
	//合成行列
	viewProjectionMatrix = Multiply(viewMatrix, projectionMatrix);
}
