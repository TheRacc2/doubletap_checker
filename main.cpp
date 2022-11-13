#include <iostream>
#include <Windows.h>
#include <vector>

class c_key {
public:
	char name;
	int vk_code;
	uint64_t last_pressed;

	c_key(char name) {
		this->name = name;
		this->vk_code = VkKeyScanExA(name, GetKeyboardLayout(NULL));
		this->last_pressed = GetTickCount64();
	}
};

int delta_time = 0;
std::vector<c_key> keys;

LRESULT CALLBACK LowLevelKeyboardProc(int code, WPARAM w_param, LPARAM l_param) {
	KBDLLHOOKSTRUCT* key_meta = (KBDLLHOOKSTRUCT*)l_param;
	
	if (w_param == WM_KEYDOWN) {
		for (c_key& key : keys) {
			if (key.vk_code == key_meta->vkCode) {
				uint64_t current_time = GetTickCount64();

				uint64_t delta = current_time - key.last_pressed;

				if (delta <= delta_time) {
					printf("Key \"%c\" double-tapped! (delta was %llums)\n", key.name, delta);
				}

				key.last_pressed = current_time;
			}
		}
	}

	return CallNextHookEx(NULL, code, w_param, l_param);
}

int main() {
	std::cout << "Enter key amount: ";

	int key_amount;
	std::cin >> key_amount;
	
	for (int i = 0; i < key_amount; i++) {
		system("cls");

		std::cout << "Enter key: ";

		char name;
		std::cin >> name;

		keys.push_back(c_key(name));
	}

	system("cls");

	std::cout << "Enter delta-time in milliseconds to record as a double-tap: ";
	std::cin >> delta_time;

	system("cls");

	HHOOK keyboard_hook = SetWindowsHookExA(WH_KEYBOARD_LL, &LowLevelKeyboardProc, GetModuleHandleA(NULL), 0);

	MSG msg;
	while (GetMessageA(&msg, NULL, 0, 0) != -1) {
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}

	UnhookWindowsHookEx(keyboard_hook);
}