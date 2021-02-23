#include <DxLib.h>
#include <cmath>
using namespace std;

typedef struct {
	float x, x1, x2, x3, x4, y, r, d1, d2, t, s;
	unsigned int c1, c2, c3;
	int n;
}ch_t;

typedef struct {
	float l, r, t, b;
	unsigned int c1;
}box_t;

char keys[256];
float ch_d, cx1, cy1, cx2, cy2;
ch_t ch;
box_t box;

float pythagoras(float x1, float y1, float x2, float y2) {
	float dx = x2 - x1;
	float dy = y2 - y1;
	return (dx * dx) - (dy * dy);
}

float radian(float n) {
	return n * 3.141592653589793f / 180.0f;
}

void setting() {
	ch.x = 320.0f;
	ch.y = 340.0f;
	ch.r = 10.0f;
	ch.d1 = 60.0f;
	ch.d2 = 120.0f;
	ch.t = 0.0f;
	ch.s = 8.0f;
	ch.c1 = GetColor(255, 0, 0);
	ch.c2 = GetColor(255, 255, 255);
	ch.c3 = GetColor(0, 128, 0);
	ch.n = 100;

	box.l = 290.0f;
	box.r = 350.0f;
	box.t = 220.0f;
	box.b = 340.0f;
	box.c1 = GetColor(128, 0, 128);
}

bool checkhit(box_t box, float cx, float cy) {
	bool result = false;
	if (cx > box.l - ch.r && cx<box.r + ch.r && cy>box.t - ch.r && cy < box.b + ch.r) {
		result = true;
		float r2 = ch.r * ch.r;
		if (cx < box.l) {
			if (cy < box.t && pythagoras(box.l, box.t, cx, cy) >= r2) {
				result = false;
			}
			else if (cy > box.b && pythagoras(box.l, box.b, cx, cy) >= r2) {
				result = false;
			}
		}
		else if (cx > box.r) {
			if (cy < box.t && pythagoras(box.r, box.t, cx, cy) >= r2) {
				result = false;
			}
			else if (cy > box.b && pythagoras(box.r, box.b, cx, cy) >= r2) {
				result = false;
			}
		}
	}
	return result;
}

void coordinate() {
	cx1 = ch.x + cos(radian(ch.t)) * ch_d;
	cy1 = ch.y + sin(radian(ch.t)) * ch_d;
	cx2 = ch.x + cos(radian(ch.t + 180.0f)) * ch_d;
	cy2 = ch.y + sin(radian(ch.t + 180.0f)) * ch_d;
}

void operation() {
	GetHitKeyStateAll(keys);
	if (keys[KEY_INPUT_RIGHT]) ch.t += ch.s;
	if (keys[KEY_INPUT_LEFT]) ch.t -= ch.s;
	if (keys[KEY_INPUT_SPACE]) {
		ch_d = ch.d2;
	}
	else {
		ch_d = ch.d1;
	}
}

void draw() {
	ClearDrawScreen();
	DrawCircleAA(ch.x, ch.y, ch.d1, ch.n, GetColor(150, 150, 150), FALSE);
	DrawCircleAA(ch.x, ch.y, ch.d2, ch.n, GetColor(150, 150, 150), FALSE);
	DrawBoxAA(box.l, box.t, box.r, box.b, box.c1, FALSE);
	if (checkhit(box, cx1, cy1)) {
		DrawCircleAA(cx1, cy1, ch.r, ch.n, ch.c3, TRUE);
	}
	else {
		DrawCircleAA(cx1, cy1, ch.r, ch.n, ch.c1, TRUE);
	}
	DrawCircleAA(cx2, cy2, ch.r, ch.n, ch.c2, TRUE);
	ScreenFlip();
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
	ChangeWindowMode(TRUE);
	setting();
	if (DxLib_Init() == -1) return -1;
	SetDrawScreen(DX_SCREEN_BACK);
	ch_d = ch.d1;
	while (true) {
		if (ProcessMessage() == -1) return -1;
		coordinate();
		draw();
		operation();
	}
	DxLib_End();
	return 0;
}