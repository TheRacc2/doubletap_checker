#include <iostream>
#include <Windows.h>
#include <vector>

class c_key {
public:
	char name;
	bool can_press;
	int vk_code;
	uint64_t last_pressed;

	c_key(char name) {
		this->name = name;
		this->vk_code = VkKeyScanExA(name, GetKeyboardLayout(NULL));
		this->last_pressed = GetTickCount64();
		this->can_press = true;
	}
};

int delta_time = 0;
int count = 1;
std::vector<c_key> keys;

LRESULT CALLBACK LowLevelKeyboardProc(int code, WPARAM w_param, LPARAM l_param) {
	KBDLLHOOKSTRUCT* key_meta = (KBDLLHOOKSTRUCT*)l_param;
	
	if (w_param == WM_KEYDOWN) {
		for (c_key& key : keys) {
			if (key.vk_code == key_meta->vkCode && key.can_press) {
				uint64_t current_time = GetTickCount64();

				uint64_t delta = current_time - key.last_pressed;

				if (delta <= delta_time) {
					std::cout << "[!] \"" << key.name << "\" double-tapped. Delta was " << delta << "ms. [" << count << "x]" << std::endl;
					count++;
				}

				key.last_pressed = current_time;
				key.can_press = false;
			}
		}
	}
	else if (w_param == WM_KEYUP) {
		for (c_key& key : keys) {
			if (key.vk_code == key_meta->vkCode) {
				key.can_press = true;
			}
		}
	}

	return CallNextHookEx(NULL, code, w_param, l_param);
}

int main() {
	SetConsoleTitleA("double-tap checker: Config");

	std::cout << "Enter key amount: ";

	int key_amount;
	std::cin >> key_amount;

	std::string new_title = "double-tap checker (";
	
	for (int i = 0; i < key_amount; i++) {
		system("cls");

		std::cout << "Enter key: ";

		char name;
		std::cin >> name;

		new_title += "\"";
		new_title += name;
		new_title += "\"";

		if (i != key_amount - 1)
			new_title += ", ";			

		keys.push_back(c_key(name));
	}

	new_title += ")";

	SetConsoleTitleA(new_title.c_str());

	system("cls");

	std::cout << "Enter delta-time in milliseconds to record as a double-tap: ";
	std::cin >> delta_time;

	system("cls");

	std::cout << "========================================" << std::endl;

	HHOOK keyboard_hook = SetWindowsHookExA(WH_KEYBOARD_LL, &LowLevelKeyboardProc, GetModuleHandleA(NULL), 0);

	MSG msg;
	while (GetMessageA(&msg, NULL, 0, 0) != -1) {
		TranslateMessage(&msg);
		DispatchMessageA(&msg);
	}

	std::cout << std::endl << "========================================";
	UnhookWindowsHookEx(keyboard_hook);
}