#include <DxLib.h>
#include <cmath>
#include <deque>
#include <tuple>
#include <vector>
using namespace std;

typedef struct {
	float x, x1, x2, x3, x4, y, cx1, cy1, cx2, cy2, r, d, d1, d2, t, s;
	unsigned int c1, c2, c3;
	int n;
}CH_T;

typedef struct {
	float x, y, cx, cy, rx1, ry1, rx2, ry2, rx3, ry3, rx4, ry4, t, w, h, sx, sy, st;
	unsigned int c;
	int type, state;
}BOX_T;

typedef tuple<BOX_T, long long> IT;
const int WIDTH = 640;
const int HEIGHT = 480;
const int BEGIN = -100;
const int END = 30;
int I;
int J;
int SCORE;
long long FLAME;
char KEYS[256];
CH_T CH;
deque<BOX_T> FLOW;
vector<vector<IT>> WAVE;
BOX_T b(int type);
void setting_wave();

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
	I = 0;
	J = 0;
	CH.x = 320.0f;
	CH.y = 340.0f;
	CH.r = 10.0f;
	CH.d = 60.0f;
	CH.d1 = 60.0f;
	CH.d2 = 120.0f;
	CH.t = 0.0f;
	CH.s = 5.0f;
	CH.c1 = GetColor(255, 0, 0);
	CH.c2 = GetColor(255, 255, 255);
	CH.c3 = GetColor(255, 215, 0);
	CH.n = 100;
}

tuple<float, float> angle_adjust(BOX_T box, float cx, float cy) {
	float x = cos(radian(360.0f - box.t)) * (cx - box.cx) - sin(radian(360.0f - box.t)) * (cy - box.cy) + box.cx;
	float y = sin(radian(360.0f - box.t)) * (cx - box.cx) + cos(radian(360.0f - box.t)) * (cy - box.cy) + box.cy;
	return make_tuple(x, y);
}

bool check_hit(BOX_T box, float cx, float cy) {
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

int check_hit_all() {
	int result = 0;
	bool check1, check2;
	for (BOX_T box : FLOW) {
		switch (box.type) {
		case 0:
			check1 = check_hit(box, CH.cx1, CH.cy1);
			check2 = check_hit(box, CH.cx2, CH.cy2);
			if (check1 && check2) {
				result = 3;
			}
			else if (check1) {
				result = 1;
			}
			else if (check2) {
				result = 2;
			}
			break;
		case 1:
		case 2:
			float cx1, cy1, cx2, cy2;
			tie(cx1, cy1) = angle_adjust(box, CH.cx1, CH.cy1);
			tie(cx2, cy2) = angle_adjust(box, CH.cx2, CH.cy2);
			check1 = check_hit(box, cx1, cy1);
			check2 = check_hit(box, cx2, cy2);
			if (check1 && check2) {
				result = 3;
			}
			else if (check1) {
				result = 1;
			}
			else if (check2) {
				result = 2;
			}
			break;
		}
	}
	return result;
}

BOX_T box_generate(int type, float x, float w, float h) {
	BOX_T box;
	switch (type) {
	case 0:
		box.x = x;
		box.y = BEGIN;
		box.w = w;
		box.h = h;
		box.sy = 4.0f;
		box.c = GetColor(0, 255, 255);
		box.type = type;
		break;
	case 1:
	case 2:
		box.x = x;
		box.y = BEGIN;
		box.w = w;
		box.h = h;
		box.cx = box.x + w / 2;
		box.cy = box.y + h / 2;
		box.rx1 = box.x;
		box.ry1 = box.y;
		box.rx2 = box.x;
		box.ry2 = box.y + box.h;
		box.rx3 = box.x + box.w;
		box.ry3 = box.y + box.h;
		box.rx4 = box.x + box.w;
		box.ry4 = box.y;
		box.t = 0.0f;
		box.st = 5.0f;
		box.sy = 4.0f;
		box.c = GetColor(128, 0, 128);
		box.type = type;
		break;
	}
	return box;
}

BOX_T box_update(BOX_T box) {
	BOX_T next = box;
	switch (box.type) {
	case 0:
		next.y += box.sy;
		break;
	case 1:
		next.y += box.sy;
		next.cy += box.sy;
		next.rx1 = (box.x - next.cx) * cos(radian(box.t)) - (box.y - next.cy) * sin(radian(box.t)) + next.cx;
		next.ry1 = (box.x - next.cx) * sin(radian(box.t)) + (box.y - next.cy) * cos(radian(box.t)) + next.cy;
		next.rx2 = (box.x - next.cx) * cos(radian(box.t)) - (box.y + box.h - next.cy) * sin(radian(box.t)) + next.cx;
		next.ry2 = (box.x - next.cx) * sin(radian(box.t)) + (box.y + box.h - next.cy) * cos(radian(box.t)) + next.cy;
		next.rx3 = (box.x + box.w - next.cx) * cos(radian(box.t)) - (box.y + box.h - next.cy) * sin(radian(box.t)) + next.cx;
		next.ry3 = (box.x + box.w - next.cx) * sin(radian(box.t)) + (box.y + box.h - next.cy) * cos(radian(box.t)) + next.cy;
		next.rx4 = (box.x + box.w - next.cx) * cos(radian(box.t)) - (box.y - next.cy) * sin(radian(box.t)) + next.cx;
		next.ry4 = (box.x + box.w - next.cx) * sin(radian(box.t)) + (box.y - next.cy) * cos(radian(box.t)) + next.cy;
		next.t += box.st;
		next.t = fmod(next.t, 360.0f);
		break;
	case 2:
		next.y += box.sy;
		next.cy += box.sy;
		next.rx1 = (box.x - next.cx) * cos(radian(box.t)) - (box.y - next.cy) * sin(radian(box.t)) + next.cx;
		next.ry1 = (box.x - next.cx) * sin(radian(box.t)) + (box.y - next.cy) * cos(radian(box.t)) + next.cy;
		next.rx2 = (box.x - next.cx) * cos(radian(box.t)) - (box.y + box.h - next.cy) * sin(radian(box.t)) + next.cx;
		next.ry2 = (box.x - next.cx) * sin(radian(box.t)) + (box.y + box.h - next.cy) * cos(radian(box.t)) + next.cy;
		next.rx3 = (box.x + box.w - next.cx) * cos(radian(box.t)) - (box.y + box.h - next.cy) * sin(radian(box.t)) + next.cx;
		next.ry3 = (box.x + box.w - next.cx) * sin(radian(box.t)) + (box.y + box.h - next.cy) * cos(radian(box.t)) + next.cy;
		next.rx4 = (box.x + box.w - next.cx) * cos(radian(box.t)) - (box.y - next.cy) * sin(radian(box.t)) + next.cx;
		next.ry4 = (box.x + box.w - next.cx) * sin(radian(box.t)) + (box.y - next.cy) * cos(radian(box.t)) + next.cy;
		next.t -= box.st;
		next.t = fmod(next.t, 360.0f);
		break;
	}
	if (next.y == CH.y) ++SCORE;
	return next;
}

void push_to_flow() {
	if (I >= WAVE.size()) return;
	int len = WAVE[I].size();
	BOX_T box;
	long long f;
	for (int j = J; j < len; ++j) {
		tie(box, f) = WAVE[I][j];
		if (f == FLAME) {
			FLOW.push_back(box);
			++J;
			if (J == len) {
				++I;
				J = 0;
				FLAME = 0;
				break;
			}
		}
		else {
			break;
		}
	}
}

void box_flow() {
	push_to_flow();
	for (int i = 0; i < FLOW.size(); ++i) {
		BOX_T box = FLOW.front();
		if (box.y < HEIGHT + END) FLOW.push_back(box_update(box));
		FLOW.pop_front();
	}
}

void box_draw() {
	for (BOX_T box : FLOW) {
		switch (box.type) {
		case 0:
			DrawBoxAA(box.x, box.y, box.x + box.w, box.y + box.h, box.c, FALSE);
			break;
		case 1:
		case 2:
			DrawQuadrangleAA(box.rx1, box.ry1, box.rx2, box.ry2, box.rx3, box.ry3, box.rx4, box.ry4, box.c, FALSE);
			break;
		}
	}
}

void ch_draw() {
	if (check_hit_all() == 1) {
		DrawCircleAA(CH.cx1, CH.cy1, CH.r, CH.n, CH.c3, TRUE);
	}
	else if (check_hit_all() == 2) {
		DrawCircleAA(CH.cx2, CH.cy2, CH.r, CH.n, CH.c3, TRUE);
	}
	else if (check_hit_all() == 3) {
		DrawCircleAA(CH.cx1, CH.cy1, CH.r, CH.n, CH.c3, TRUE);
		DrawCircleAA(CH.cx2, CH.cy2, CH.r, CH.n, CH.c3, TRUE);
	}
	DrawCircleAA(CH.cx1, CH.cy1, CH.r, CH.n, CH.c1, FALSE);
	DrawCircleAA(CH.cx2, CH.cy2, CH.r, CH.n, CH.c2, FALSE);
}

void ch_coordinate() {
	CH.cx1 = CH.x + cos(radian(CH.t)) * CH.d;
	CH.cy1 = CH.y + sin(radian(CH.t)) * CH.d;
	CH.cx2 = CH.x + cos(radian(CH.t + 180.0f)) * CH.d;
	CH.cy2 = CH.y + sin(radian(CH.t + 180.0f)) * CH.d;
}

void draw() {
	ClearDrawScreen();
	DrawFormatString(0, 0, GetColor(255, 255, 255), "SCORE: %d", SCORE);
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
	setting_wave();
	while (true) {
		if (ProcessMessage() == -1) return -1;
		ch_coordinate();
		box_flow();
		draw();
		operation();
		++FLAME;
	}
	DxLib_End();
	return 0;
}

/* ↓↓譜面設定 */

BOX_T b(int type) {
	BOX_T box;
	switch (type) {
	case 0:
		box = box_generate(0, 280.0f, 80.0f, 20.0f);
		break;
	case 1:
		box = box_generate(0, 320.0f, 120.0f, 20.0f);
		break;
	case 2:
		box = box_generate(0, 200.0f, 120.0f, 20.0f);
		break;
	case 3:
		box = box_generate(1, 245.0f, 150.0f, 20.0f);
		break;
	case 4:
		box = box_generate(2, 245.0f, 150.0f, 20.0f);
		break;
	case 5:
		box = box_generate(0, 220.0f, 200.0f, 20.0f);
		break;
	}
	return box;
}

void setting_wave() {
	WAVE = {
		// ウェーブ0
		{
			make_tuple(b(0), 0),
			make_tuple(b(1), 100),
			make_tuple(b(1), 136),
			make_tuple(b(1), 172),
			make_tuple(b(1), 208),
			make_tuple(b(4), 258),
			make_tuple(b(2), 308),
			make_tuple(b(2), 344),
			make_tuple(b(2), 380),
			make_tuple(b(2), 416),
			make_tuple(b(5), 466)
		},
		// ウェーブ1
		{
			make_tuple(b(0), 100),
			make_tuple(b(0), 110),
			make_tuple(b(2), 210),
			make_tuple(b(2), 246),
			make_tuple(b(0), 296),
			make_tuple(b(5), 332),
			make_tuple(b(1), 368),
			make_tuple(b(2), 404),
			make_tuple(b(3), 454),
			make_tuple(b(4), 504),
			make_tuple(b(0), 540),
			make_tuple(b(3), 590),
			make_tuple(b(5), 640)
		}
	};
}
