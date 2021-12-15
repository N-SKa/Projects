

// [Window Handle]

const int VLC = 4;	// Var line counter.


HWND hGF_B;	// GetFile_Button Handle

HWND hCT_C;	// ChooseType_Combo Handle
HWND hWFPS_E;	// WriteFilePathNameSingle_Edit Handle
HWND hWFP_E[3];	// WriteFilePath_Edit Handle
HWND hWFP_Max_E;	// WriteFilePathNameMax_Edit Handle
HWND hRFP_B;	// ResetFilPath_Button Handle

HWND hWH_E;	// WriteHeader_Edit Handle
HWND hWF_E;	// WriteFooter_Edit Handle

HWND hWR_E;	// WriteResouce_Edit Handle
HWND hRR_B;	// ResetResource_Button Handle
HWND hSR_B;	// SetResource_Button Handle
HWND hSVR_B;	// SetVarResouce_Button Handle

HWND hWU_E;	// WriteURL_Edit Handle
HWND hRU_B;	// ResetURL_Button Handle
HWND hSVU_B;	// SetVarURL_Button Handle


HWND hCVT_C[VLC + 1];	// ChooseVarType_Combo Handle
HWND hWV_Min_E[VLC + 1];	// WriteVar_Min_Edit Handle
HWND hWV_Max_E[VLC + 1];	// WriteVar_Max_Edit Handle


HWND hWVR_SP_F_E[VLC + 1];	// WriteVarResource_StartPosition_Front_Edit Handle
HWND hWVR_SP_B_E[VLC + 1];	// WriteVarResource_StartPosition_Behind_Edit Handle
HWND hWVR_F_E[VLC + 1];	// WriteVarResource_Front_Edit Handle
HWND hWVR_B_E[VLC + 1];	// WriteVarResource_Behind_Edit Handle


HWND hDVL_B[VLC + 1];	// DeleteVarLine_Button Handle
HWND hRVL_B[VLC + 1];	// ResetVarLine_Button Handle




// [CONSTANT]

const int PMX = GetSystemMetrics(SM_CXSCREEN);	// PrimaryMonitor X.
const int PMY = GetSystemMetrics(SM_CYSCREEN);	// PrimaryMonitor Y.


#ifndef H_BMAX
#define H_BMAX 0xffff	// High Max bytes.
#endif
#ifndef BMAX
#define BMAX 0xff	// Max bytes.
#endif
#ifndef L_BMAX
#define L_BMAX 0xf	// Low Max bytes.
#endif

enum WindowSize : int
{
	MARGIN = 8,	// Inline margin.

	MF_WIDTH = 650,	// MainFrame Width.
	MF_HEIGHT = 400,	// MainFrame Height.
	//const int MF_rWIDT	// MainFrame Real Height.
	// = MF_WIDTH - 25	// Incomprehensible 25 pixels.
	//const int MF_rHEIGH	// MainFrame Real Height.
	// = MF_HEIGHT - 59	// Incomprehensible 59 pixels.
	// + GetSystemMetrics(SM_CYCAPTION)	// Titlebar Height (=23).
	// + GetSystemMetrics(SM_CYMENU)	// Menubar Height (=20).

	B_WIDTH = 70,	// Button Width.
	B_HEIGHT = 20,	// Button Height.

	V_C_WIDTH = 100,	// Var Combo Width.

	V_E_WIDTH = 60,	// Var Edit Width.
	E_HEIGHT = 22	// nHeight >= 22.
};




///// [UserDef] /////

bool FilePathGroup::GetFile()
{
	///// <Preparation> { /////

	bool return_bool = false;

	InetResourceOutput iro;

	int dl_type = (int)SendMessage(hCT_C, CB_GETCURSEL, 0, 0);	// Download type.

	stringT t_ts;
	LPTSTR t_tc = new TCHAR[BMAX];

	///// } <Preparation> /////



	// Make file path.
	struct
	{
		stringT operator () (int varFPm)
		{
			stringT return_fn;

			int t_dl_type = (int)SendMessage(hCT_C, CB_GETCURSEL, 0, 0);
			stringT t_ts;

			TCHAR t_path_f[BMAX];	// front path.
			TCHAR t_path_m[L_BMAX];	// middle path.
			TCHAR t_path_b[BMAX];	// behind path.

			if (t_dl_type == 1 || varFPm == -1) {
				GetWindowText(hWFPS_E, t_path_f, BMAX);

				return_fn = t_path_f;
			} else {
				GetWindowText(hWFP_E[0], t_path_f, BMAX);
				GetWindowText(hWFP_E[1], t_path_m, L_BMAX);
				GetWindowText(hWFP_E[2], t_path_b, BMAX);

				t_ts = StringOverDigits(t_path_m, L_BMAX, Ttoi(t_path_m) + varFPm);

				return_fn = t_path_f;
				return_fn += t_ts;
				return_fn += t_path_b;
			}

			return return_fn;
		}
	} FilePath;




	if (dl_type == 1) {	// Single.

		GetWindowText(hWU_E, t_tc, BMAX);
		return_bool = iro.NoDAT_InetToFile(t_tc, FilePath(-1).c_str());

	} else {	// Multiple, +Resource, dlSource.

		///// <Preparation - for loop> { /////

		FilePathGroup fpg;
		ResouceGroup rg;
		VarLineGroup vlg;
		stringT
			fp_fin,	// Final file path.
			url_fin,	// // Final URL.
			ts_buf[VLC];	// Resource buffer.
		const int
			UV_LEN = clenT(vlg.MVTN(0, 1).c_str()),	// URL var length.
			RV_LEN = clenT(vlg.MVTN(1, 1).c_str());	// Resource var length.
		bool
			b_loop_counter[VLC],	// Still loop or not.
			b_grf[VLC];	// Get resource first or not.
		int
			urlVarLine = 0,		// Number of variable line for URL.
			i_loop_min[VLC],	// Min (line type 0) for loop counter.
			i_loop_max[VLC],	// Max (line type 0) for loop counter.
			ln_num = 0,	// Line number.
			lc_var[VLC];	// loop counter var.
		unsigned int fp_var = 0;	// Filepath var.

		GetWindowText(hWFP_Max_E, t_tc, BMAX);
		unsigned int FP_MAX = Ttoi(t_tc);
		GetWindowText(hWFP_E[1], t_tc, BMAX);
		FP_MAX -= Ttoi(t_tc);	// FilePath_middle Max.

		///// } <Preparation - for loop> /////




		///// <Initialize - for loop> { /////
		
		// i_loop_min : When URL var : min_var, Res var : -1, else : -2.
		// i_loop_max : i_loop_min == min_var : max_var.
		for (int i = 0, j = 0; i < VLC; i++) {
			j = (int)SendMessage(hCVT_C[i], CB_GETCURSEL, 0, 0);
			GetWindowText(hWV_Min_E[i], t_tc, L_BMAX);
			i_loop_min[i] = (SendMessage(hCVT_C[i], CB_GETLBTEXTLEN, j, 0) == UV_LEN) ?
              Ttoi(t_tc) :
							((SendMessage(hCVT_C[i], CB_GETLBTEXTLEN, j, 0) == RV_LEN) ? -1 : -2);
			if (i_loop_min[i] >= 0) {
				GetWindowText(hWV_Max_E[i], t_tc, L_BMAX);
				i_loop_max[i] = Ttoi(t_tc);
			} else {
				i_loop_max[i] = -2;
			}
		}


		// There are no lines, return false.
		if (i_loop_min[0] == -2) {
			MessageBox(nullptr, _T("Please make and set variables."), _T(""), MB_OK);
			return false;
		}

		
		// Array.
		for (int i = 0; i < VLC; i++) {
			if (i_loop_min[i] != -2) {
				b_loop_counter[i] = true;
				b_grf[i] = true;
				lc_var[i] = 0;
			} else {	// When line does not exist.
				b_loop_counter[i] = false;
				b_grf[i] = false;
				lc_var[i] = 0;
			}
		}


		// URL.
		GetWindowText(hWU_E, t_tc, BMAX);
		url_fin = t_tc;

		///// } <Initialize - for loop> /////




		do {

			if (
				i_loop_min[ln_num] >= 0 &&	// When line = URL var.
				b_loop_counter[ln_num] == true)
			{

				GetWindowText(hWU_E, t_tc, BMAX);
				url_fin = fpg.SetURL_Multiple(t_tc, ln_num, lc_var[ln_num]);

				// Sub Loop Finalize. If line is over.
				if (i_loop_min[ln_num] > i_loop_max[ln_num]) {
					b_loop_counter[ln_num] = false;
					ln_num -= 1;
				} else {
					i_loop_min[ln_num] += 1;
					lc_var[ln_num] += 1;
					//if (b_loop_counter[ln_num] == true && b_loop_counter[ln_num + 1] == false) {
					b_loop_counter[ln_num + 1] = true;
				}


			} else if (
				i_loop_min[ln_num] == -1 &&	// When line = Res var.
				b_loop_counter[ln_num] == true)
			{


				int varLn;
				for (varLn = ln_num; varLn >= 0 && i_loop_min[varLn] < 0; varLn--);

				// Set resource.
				if (b_grf[ln_num] == true) {
					if (varLn != -1) {	// When URL has variables.
						iro.createBufferFile(url_fin.c_str());
					} else {	// When URL does not have variables.
						iro.createBufferFile(url_fin.c_str());
					}
					ts_buf[ln_num] = iro.getResource();
					b_grf[ln_num] = false;
				}




				///// <Reposition resource buffer> { /////

				url_fin = fpg.SetURL_PlusResource(ts_buf[ln_num], ln_num);
				lc_var[ln_num] += 1;


				GetWindowText(hWVR_B_E[ln_num], t_tc, BMAX);
				int t_spos = (int)ts_buf[ln_num].find(t_tc);	// substr start pos.


				// Sub Loop Finalize. If line is over.
				if (t_spos == string::npos) {
					return_bool = fpg.SubLoopFin(ln_num, ture, true);
				} else {
					return_bool = fpg.SubLoopFin(ln_num, false, true);
				}

				///// } <Reposition resource buffer> /////


			} else if (i_loop_min[ln_num] == -2) {	// When line = NO var.

				return_bool = fpg.SubLoopFin(-2, true, true);

			}




			// Go to sub loop, or download file.
			if (b_loop_counter[ln_num + 1] == true) {
					return_bool = fpg.SubLoopFin(ln_num, false, false);
			} else if (b_loop_counter[ln_num + 1] == false) {
				// Create file path and download file.
				fp_fin = FilePath(fp_var);
				if (url_fin != _T("")) {	// If URL is void, to the next step.
					return_bool = iro.DLFile(url_fin, fp_fin, dl_type);
				}
			}

		} while (b_loop_counter[0]);
	}


	// Finalize.
	delete[] t_tc;

	if (return_bool) {	// When succeeded.
		MessageBox(nullptr, _T("SUCCEEDED."), _T(""), MB_OK);
	} else {	// When failed.
		MessageBox(nullptr, _T("FAILED."), _T(""), MB_OK);
	}

	return return_bool;
}




void URLGroup::SetVarURL()
{
	VarLineGroup vlg;

	int b_cnt = 0;	// Blackets counter.
	TCHAR t_tc[BMAX];
	stringT
		t_ts,
		ts_url;

	GetWindowText(hWU_E, t_tc, BMAX);
	ts_url = t_tc;


	long long int bpos_s = ts_url.find(_T("["));	// Blackets position start.
	long long int bpos_e = ts_url.find(_T("]"));	// Blackets position end.

	///// <Exception handling - Check []> { /////

	if (bpos_s == stringT::npos &&	// [ exists.
		bpos_e == stringT::npos)	// ] exists.
	{
		MessageBox(nullptr, _T("URL does not have variables."), _T(""), MB_OK);
		return;
	}

	for (int i = 0, j = 0, check_digit = 0; i < clenT(t_tc); i++) {
		if (ts_url.substr(i, 1) == (stringT)_T("[")) {
			check_digit++;
			b_cnt++;	// Count blackets.
		} else if (ts_url.substr(i, 1) == (stringT)_T("]")) {
			if (check_digit == 1) { j++; }
			check_digit--;
		}

		if ((i == clenT(t_tc) -1 && (check_digit != 0 || j > VLC)) ||	// When end at [, or [] are over VLC.
			check_digit == -1 ||	// When ] is ahead [.
			check_digit == 2) 	// When [ after [.
		{
			MessageBox(nullptr, _T("There are wrong blackets,\nor too many variables for number of lines."), _T(""), MB_OK);
			return;
		}
	}

	///// } <Exception handling - Check []> /////




	///// <Show line for Var> { /////

	for (int i = 0, j = 0; i < VLC; i++, j++) {

		// If resource line is visible, detective line shift to under.
		for (int h = 0; IsWindowVisible(hWVR_SP_F_E[j]) == 1; h++) {
			j++;
		}

		if (ts_url.find(_T("["), bpos_s) != stringT::npos) {

			if (j >= VLC) {	// When var is over line counts.
				MessageBox(nullptr, _T("There are too many variables for number of lines."), _T(""), MB_OK);
				return;
			}else if (IsWindowVisible(hDVL_B[j]) == 0) {
				vlg.ShowVarLine(0, j);
			}
			bpos_s = ts_url.find(_T("["), bpos_s + 1);	// Blackets reposition start.

		} else {

			// Delete excess line.
			for (int h = j; h < VLC; j++, h++) {
				if (IsWindowVisible(hWV_Min_E[h]) == 1) {
					if (IsWindowVisible(hWVR_SP_F_E[h + 1]) == 0) {	// If there is resource line under var line,
						vlg.HideVarLine(j);
						i = VLC;
					} else if (IsWindowVisible(hWVR_SP_F_E[h + 1]) == 1) {	// If there is NO resource line under var line,
						vlg.DeleteVarLine(j);
						i = VLC;
					}
				}
			}

		}
	}

	bpos_s = ts_url.find(_T("["));	// Blackets reposition start.

	///// } <Show line for Var> /////




	///// <Set variable name into the type combo box> { /////

	for (int i = 0; i < b_cnt; i++) {
		for (int j = 0; j < VLC; j++) {
			GetWindowText(hCVT_C[j], t_tc, BMAX);
			t_ts = t_tc;
			if (t_ts == vlg.MVTN(0, i + 1)) {
				break;
			} else if (t_ts == _T("") && t_ts != vlg.MVTN(0, i + 1)) {
				vlg.SVIC(0, i + 1, j);
				break;
			}
		}
	}

	///// } <Set variable name into the type combo box> /////




	///// <Replace URL with VAR> { /////

	for (int i = 0; i <= b_cnt; i++)
	{
		for (int j = 0; j <= b_cnt; j++) {
			if (j == 0) {
				for (int k = 0; k <= b_cnt; k++) {

					// Make "[VARx]" for search.
					t_ts = _T("[");
					t_ts += vlg.MVTN(0, k + 1);
					t_ts += _T("]");

					if (bpos_s == ts_url.find(t_ts)) {
						bpos_s = ts_url.find(_T("["), bpos_s + 1);	// Blackets reposition start + 1.
						bpos_e = ts_url.find(_T("]"), bpos_e + 1);	// Blackets reposition end + 1.
						j = b_cnt;
						break;
					}
				}
			} else {
				for (int k = 0; k < b_cnt; k++) {

					// Make "[VARx]" for search.
					t_ts = _T("[");
					t_ts += vlg.MVTN(0, k + 1);
					t_ts += _T("]");

					if (ts_url.find(t_ts) == stringT::npos) {
						// Replace.
						t_ts = ts_url.substr(bpos_e + 1);
						ts_url = ts_url.substr(0, bpos_s);
						ts_url += _T("[");
						ts_url += vlg.MVTN(0, k + 1);
						ts_url += _T("]");
						ts_url += t_ts;
						SetWindowText(hWU_E, ts_url.c_str());

						// Reposition blackets for search.
						bpos_s = ts_url.find(_T("["), bpos_s + 1);	// Blackets reposition start + 1.
						bpos_e = ts_url.find(_T("]"), bpos_e + 1 + 4);	// Blackets reposition end + 1 + "VARx"(4cs).
						j = b_cnt;
						break;
					}
				}
			}
		}
	}

	///// } <Replace URL with VAR> /////
}




LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);

		ChangeForm cf;

		switch (wmId)
		{
		case ID_CT_C:
			switch (HIWORD(wParam)) {
			case CBN_SELCHANGE :
				cf.SwitchForm((int)SendMessage(hCT_C, CB_GETCURSEL, 0, 0));
			}
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		EndPaint(hWnd, &ps);
	}
	break;
  
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
    
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}
