
#pragma once


///// [Preprocessor] /////

#ifndef H_BMAX
#define H_BMAX 0xffff	// High Max bytes.
#endif
#ifndef BMAX
#define BMAX 0xff	// Max bytes.
#endif
#ifndef L_BMAX
#define L_BMAX 0xf	// Low Max bytes.
#endif

#define mci_BS 0x4000
#define fDAT "BUF.DAT"
#define TfDAT _T(fDAT)




stringT InetResourceOutput::getResource()
{
	stringT
		return_ts,
		t_ts;
	string t_s;
	char t_c[BMAX];
	TCHAR t_tc[sizeof(t_c)*2];

	int charset_pos = 0;

	FILE* t_pf;


	if (fopenT(&t_pf, TfDAT, _T("r, ccs=UTF-8")) == EINVAL) { return_ts = _T(""); return return_ts; }
	if (t_pf == nullptr) { return_ts = _T(""); return return_ts; }


	while (fgetsT(t_tc, BMAX*2, t_pf)) {
		return_ts += t_tc;
	}




	///// <Check charset in html source> { /////

	if (return_ts.find(_T("charset")) != std::stringT::npos) {
		charset_pos = (int)return_ts.find(_T("charset")) + 7;
	} else if (return_ts.find(_T("CHARSET")) == std::stringT::npos) {
		charset_pos = (int)return_ts.find(_T("CHARSET")) + 7;
	} else if (return_ts.find(_T("CharSet")) == std::stringT::npos) {
		charset_pos = (int)return_ts.find(_T("CharSet")) + 7;
	}

	if (charset_pos != 0) {

		t_ts = return_ts.substr(charset_pos, 17);
		transform(t_ts.begin(), t_ts.end(), t_ts.begin(), ::tolower);	// Change characters to lower for search charset.


		// Remake per charset.
		if (t_ts.find(_T("utf-8")) != std::stringT::npos) {	// When "utf-8".
		} else if (t_ts.find(_T("shift_jis")) != std::stringT::npos) {	// When "shift_jis".
			std::ifstream ifs(fDAT);
			t_s = string((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
			return_ts = SwitchStrType(t_s);
		}
	}

	///// } <Check charset in html source> /////




	fclose(t_pf);


	return return_ts;
}
