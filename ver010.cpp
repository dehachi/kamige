#include <DxLib.h>
#include <cmath>
#include <deque>
using namespace std;

typedef struct {
	float x, x1, x2, x3, x4, y, cx1, cy1, cx2, cy2, r, d, d1, d2, t, s;
	unsigned int c1, c2, c3;
	int n;
}ch_t;

typedef struct {
	float x, y, w, h, sx, sy;
	unsigned int c;
	int type, state;
}box_t;

const int WIDTH = 640;
const int HEIGHT = 480;
const int BEGIN = -100;
const int END = 100;
long long FLAME;
char KEYS[256];
ch_t CH;
deque<box_t> FLOW;

float pythagoras(float x1, float y1, float x2, float y2) {
	float dx = x2 - x1;
	float dy = y2 - y1;
	return (dx * dx) - (dy * dy);
}

float radian(float n) {
	return n * 3.141592653589793f / 180.0f;
}

void setting() {
	FLAME = 0;
	CH.x = 320.0f;
	CH.y = 340.0f;
	CH.r = 10.0f;
	CH.d = 60.0f;
	CH.d1 = 60.0f;
	CH.d2 = 120.0f;
	CH.t = 0.0f;
	CH.s = 9.0f;
	CH.c1 = GetColor(255, 0, 0);
	CH.c2 = GetColor(255, 255, 255);
	CH.c3 = GetColor(0, 128, 0);
	CH.n = 100;
}

bool check_hit(box_t box, float cx, float cy) {
	bool result = false;
	if (cx > box.x - CH.r && cx < box.x + box.w + CH.r && cy > box.y - CH.r && cy < box.y + box.h + CH.r) {
		result = true;
		float r2 = CH.r * CH.r;
		if (cx < box.x) {
			if (cy < box.y && pythagoras(box.x, box.y, cx, cy) >= r2) {
				result = false;
			}
			else if (cy > box.y + box.h && pythagoras(box.x, box.y + box.h, cx, cy) >= r2) {
				result = false;
			}
		}
		else if (cx > box.x + box.w) {
			if (cy < box.y && pythagoras(box.x + box.w, box.y, cx, cy) >= r2) {
				result = false;
			}
			else if (cy > box.y + box.h && pythagoras(box.x + box.w, box.y + box.h, cx, cy) >= r2) {
				result = false;
			}
		}
	}
	return result;
}

bool check_hit_all() {
	bool result = false;
	for (box_t box : FLOW) {
		if (check_hit(box, CH.cx1, CH.cy1) || check_hit(box, CH.cx2, CH.cy2)) {
			result = true;
			break;
		}
	}
	return result;
}

box_t box_generate(int type, float x) {
	box_t box;
	switch (type) {
	case 0:
		box.x = x;
		box.y = BEGIN;
		box.w = 120.0f;
		box.h = 30.0f;
		box.sx = 0.0f;
		box.sy = 9.0f;
		box.c = GetColor(0, 255, 255);
		box.type = type;
		box.state = 0;
		break;
	}
	return box;
}

box_t box_update(box_t box) {
	box_t next = box;
	switch (box.type) {
	case 0:
		next.y = box.y + box.sy;
		break;
	}
	return next;
}

void box_flow() {
	if (FLAME % 20 == 0) FLOW.push_back(box_generate(0, 320.0f));
	for (int i=0; i<FLOW.size(); ++i) {
		box_t box = FLOW.front();
		if (box.y < HEIGHT + END) FLOW.push_back(box_update(box));
		FLOW.pop_front();
	}
}

void box_draw() {
	for (box_t box : FLOW) {
		DrawBoxAA(box.x, box.y, box.x + box.w, box.y + box.h, box.c, FALSE);
	}
}

void ch_draw() {
	if (!check_hit_all()) {
		DrawCircleAA(CH.cx1, CH.cy1, CH.r, CH.n, CH.c1, TRUE);
		DrawCircleAA(CH.cx2, CH.cy2, CH.r, CH.n, CH.c2, TRUE);
	}
	else {
		DrawCircleAA(CH.cx1, CH.cy1, CH.r, CH.n, CH.c3, TRUE);
		DrawCircleAA(CH.cx2, CH.cy2, CH.r, CH.n, CH.c2, TRUE);
	}
}

void coordinate() {
	CH.cx1 = CH.x + cos(radian(CH.t)) * CH.d;
	CH.cy1 = CH.y + sin(radian(CH.t)) * CH.d;
	CH.cx2 = CH.x + cos(radian(CH.t + 180.0f)) * CH.d;
	CH.cy2 = CH.y + sin(radian(CH.t + 180.0f)) * CH.d;
}

void draw() {
	ClearDrawScreen();
	DrawFormatString(0, 0, GetColor(255, 255, 255), "flame: %d", FLAME);
	DrawFormatString(0, 20, GetColor(255, 255, 255), "touch: %d", check_hit_all());
	DrawCircleAA(CH.x, CH.y, CH.d1, CH.n, GetColor(150, 150, 150), FALSE);
	DrawCircleAA(CH.x, CH.y, CH.d2, CH.n, GetColor(150, 150, 150), FALSE);
	box_draw();
	ch_draw();
	ScreenFlip();
}

void operation() {
	GetHitKeyStateAll(KEYS);
	if (KEYS[KEY_INPUT_RIGHT]) CH.t += CH.s;
	if (KEYS[KEY_INPUT_LEFT]) CH.t -= CH.s;
	if (KEYS[KEY_INPUT_SPACE]) {
		CH.d = CH.d2;
	}
	else {
		CH.d = CH.d1;
	}
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
	ChangeWindowMode(TRUE);
	SetWindowSize(WIDTH, HEIGHT);
	setting();
	if (DxLib_Init() == -1) return -1;
	SetDrawScreen(DX_SCREEN_BACK);
	while (true) {
		if (ProcessMessage() == -1) return -1;
		coordinate();
		box_flow();
		draw();
		operation();
		++FLAME;
	}
	DxLib_End();
	return 0;
}
