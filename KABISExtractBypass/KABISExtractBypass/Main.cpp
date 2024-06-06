#include "Include.h"

wstring ToLowerCase(wstring wStrText)
{
	for (int i = 0; i < wStrText.length(); i++)
		wStrText[i] = towlower(wStrText[i]);

	return wStrText;
}

HMODULE GetBaseModuleHandle(wstring wStrModule, DWORD dwProcessId)
{
	HMODULE hModule = NULL;
	HANDLE hToolHelp32 = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);

	if (hToolHelp32 != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 me32;
		me32.dwSize = sizeof(me32);

		if (Module32First(hToolHelp32, &me32))
		{
			do
			{
				if (wcsstr(ToLowerCase(me32.szModule).c_str(), ToLowerCase(wStrModule).c_str()))
				{
					hModule = me32.hModule;
					break;
				}
			} while (Module32Next(hToolHelp32, &me32));
		}
	}

	CloseHandle(hToolHelp32);
	return hModule;
}

int main()
{
	HWND hKabisWnd = NULL;

	DWORD dwStatus = NULL;
	DWORD dwRecords = NULL;
	DWORD dwInstructs = NULL;

	HMODULE hKabis = NULL;
	HANDLE hProcess = NULL;
	DWORD dwProcessId = NULL;

	int iRecords = -1;
	int iCurRecords = -1;
	bool bSaveRecords = true;

	int iCount = 0;
	int iStatus = 0;
	int iProgress = 0;
	bool bSaveStatus = false;

	setlocale(LC_ALL, "");
	SetConsoleTitleA("KABISExtractBypass by kek");

	cout << "������� ������� \"Space\" ����� ��������� ������ ���������...\n\n";
	cout << "-----------------------------------------------------------\n";
	cout << "����� ���� ���������";

	while (true)
	{
		if (!IsWindow(hKabisWnd))
		{
			hKabisWnd = FindWindowA("ThunderRT6FormDC", NULL);

			Sleep(250);
			cout << ".";

			if (IsWindow(hKabisWnd))
			{
				cout << "\n���� ��������� ���� �������!\n";
				GetWindowThreadProcessId(hKabisWnd, &dwProcessId);

				if (dwProcessId)
				{
					cout << "������������� ��������: [" << dwProcessId << "]\n";
					hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, dwProcessId);

					if (hProcess)
					{
						hKabis = GetBaseModuleHandle(L"KABIS.EXE", dwProcessId);

						if (hKabis)
						{
							cout << "������������� ������: [" << hKabis << "]\n";
							cout << "-----------------------------------------------------------\n";

						ReadMem:

							if (ReadProcessMemory(hProcess, (LPVOID)((DWORD)hKabis + 0x30A384), &dwRecords, 4, NULL))
							{
								if (ReadProcessMemory(hProcess, (LPVOID)((DWORD)dwRecords + 0x44), &dwRecords, 4, NULL))
								{
									cout << "\n[+] ����������:\n";
									cout << "������� � ������� ��������: [" << dwRecords << "]\n";

									int iResult = 0;
									DWORD dwOldProtect = NULL;

									iResult += ReadProcessMemory(hProcess, (LPVOID)((DWORD)hKabis + 0x2AA05B + 1), &dwInstructs, 4, NULL);
									iResult += VirtualProtectEx(hProcess, (LPVOID)((DWORD)hKabis + 0x2AA05B + 1), 4, PAGE_EXECUTE_READWRITE, &dwOldProtect);
									iResult += WriteProcessMemory(hProcess, (LPVOID)((DWORD)hKabis + 0x2AA05B + 1), &dwRecords, 4, NULL);
									iResult += VirtualProtectEx(hProcess, (LPVOID)((DWORD)hKabis + 0x2AA05B + 1), 4, dwOldProtect, &dwOldProtect);

									dwRecords = ((int)dwRecords + 1);

									iResult += ReadProcessMemory(hProcess, (LPVOID)((DWORD)hKabis + 0x2AA58C + 3), &dwInstructs, 4, NULL);
									iResult += VirtualProtectEx(hProcess, (LPVOID)((DWORD)hKabis + 0x2AA58C + 3), 4, PAGE_EXECUTE_READWRITE, &dwOldProtect);
									iResult += WriteProcessMemory(hProcess, (LPVOID)((DWORD)hKabis + 0x2AA58C + 3), &dwRecords, 4, NULL);
									iResult += VirtualProtectEx(hProcess, (LPVOID)((DWORD)hKabis + 0x2AA58C + 3), 4, dwOldProtect, &dwOldProtect);

									if (iResult >= 8)
									{
										char chWndText[500];
										GetWindowTextA(hKabisWnd, chWndText, sizeof(chWndText));

										if (string(chWndText).find("- Patched by kek") == string::npos)
										{
											string strSetTitle = (string(chWndText) + " - Patched by kek");
											SetWindowTextA(hKabisWnd, strSetTitle.c_str());
										}

										cout << "��� ���������� ���� ������� ����������!\n������ �� ������ ��������� ��������� ��� ��������� ������ � ������ ��������.\n";
									}
									else
										cout << "�� ������� ���������� ���� ��� ��������� ����������.\n";

									iRecords = (dwRecords - 1);
									iCurRecords = iRecords;
								}
								else
								{
									cout << "������ ��� ������ ������!\n";
									break;
								}
							}
							else
							{
								cout << "������ ��� ������ ������!\n";
								break;
							}
						}
						else
						{
							cout << "�� ������� ����� ������ ���������.\n";
							break;
						}
					}
					else
					{
						cout << "�� ������� �������� ������ � ��������!\n";
						break;
					}
				}
				else
				{
					cout << "�� ������� �������� ������������� ��������.\n";
					break;
				}
			}
		}
		else
		{
			Sleep(500);

			if (iCurRecords != iRecords)
				bSaveRecords = true;
			else
			{
				bSaveRecords = false;

				iStatus += ReadProcessMemory(hProcess, (LPVOID)((DWORD)hKabis + 0x0006CBD0), &dwStatus, 4, NULL);
				iStatus += ReadProcessMemory(hProcess, (LPVOID)((DWORD)dwStatus + 0x104), &dwStatus, 4, NULL);

				if (iStatus >= 2)
				{
					if (bSaveStatus)
					{
						iCount = 0;

						iProgress = dwStatus;
						bSaveStatus = false;

						float fPercent = ((100.0f / iCurRecords) * iProgress);
						cout << "\r��������� ���� " << iProgress << " �� " << iCurRecords << " (" << round(fPercent) << "%)";

						if (iProgress >= iCurRecords)
							cout << "\n";
					}
					else
					{
						if (iProgress != dwStatus)
						{
							if (dwStatus >= 1000
								&& dwStatus < 500000)
							{
								if (iCount >= 10)
									bSaveStatus = true;
							}

							iCount++;
						}
						else
							iCount = 0;
					}
				}

				if (ReadProcessMemory(hProcess, (LPVOID)((DWORD)hKabis + 0x30A384), &dwRecords, 4, NULL))
				{
					if (ReadProcessMemory(hProcess, (LPVOID)((DWORD)dwRecords + 0x44), &dwRecords, 4, NULL))
						iCurRecords = dwRecords;
					else
					{
						cout << "������ ��� ������ ������!\n";
						break;
					}
				}
				else
				{
					cout << "������ ��� ������ ������!\n";
					break;
				}
			}

			if (bSaveRecords)
			{
				CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
				GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbiInfo);

				if (csbiInfo.dwCursorPosition.X != 0)
					cout << "\n";

				cout << "���������� � �������� ���� ���������!\n";
				goto ReadMem;
			}
		}

		if (GetAsyncKeyState(VK_SPACE))
		{
			if (GetForegroundWindow() == GetConsoleWindow())
				break;
			else
				continue;
		}
	}

	cout << "\n";

	system("echo ���������� ��������� ���� ���������.");
	system("pause");

	return 0;
}