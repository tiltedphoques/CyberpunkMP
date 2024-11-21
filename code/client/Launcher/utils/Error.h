#pragma once

#include <Windows.h>
#include <Commctrl.h>

#include <Core/Stl.h>

void Die(const wchar_t* aText, bool aKillNow = false);

void ShowProgressDialog();
