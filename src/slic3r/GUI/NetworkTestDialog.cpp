#ifdef __WINDOWS__
#include <windows.h>
#endif

#include "NetworkTestDialog.hpp"
#include "I18N.hpp"

#include "libslic3r/Utils.hpp"
#include "GUI.hpp"
#include "GUI_App.hpp"
#include "I18N.hpp"
#include "slic3r/Utils/Http.hpp"
#include "libslic3r/AppConfig.hpp"
#include <boost/asio/ip/address.hpp>
#include <boost/log/trivial.hpp>

namespace Slic3r {
namespace GUI {

#ifdef __WINDOWS__
namespace WindowsUtils {

static const int WINDOWS_SERVER2016_BUILD = 14393;
static const int WINDOWS_SERVER2019_BUILD = 17763;
static const int WINDOWS_SERVER2022_BUILD = 20348;
static const int WINDOWS_SERVER2025_BUILD = 26100;
static const int FIRST_WINDOWS11_BUILD = 22000;
static OSVERSIONINFOEXW getWindowsVersionInfo()
{
    OSVERSIONINFOEXW osvi;
    ::ZeroMemory(&osvi, sizeof(osvi));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);

    HMODULE hNtdll = ::GetModuleHandleW(L"ntdll.dll");
    if (hNtdll)
    {
        typedef LONG (WINAPI *RtlGetVersion_t)(OSVERSIONINFOEXW*);
        RtlGetVersion_t pRtlGetVersion = reinterpret_cast<RtlGetVersion_t>(
            ::GetProcAddress(hNtdll, "RtlGetVersion")
        );

        if (pRtlGetVersion
            && pRtlGetVersion(&osvi) == 0)
        {
            return osvi;
        }
    }

    ::GetVersionExW(reinterpret_cast<OSVERSIONINFOW *>(&osvi));
    return osvi;
}

static int isWindowsServer()
{
#ifdef VER_NT_WORKSTATION
    switch ( getWindowsVersionInfo().wProductType )
    {
        case VER_NT_WORKSTATION:
            return 0;

        case VER_NT_SERVER:
        case VER_NT_DOMAIN_CONTROLLER:
            return 1;
    }
#endif // VER_NT_WORKSTATION
    return -1;
}

static bool isPlatform64Bit()
{
#if defined(__WIN64__)
    return true;
#else // Win32
    typedef BOOL (WINAPI *IsWow64Process_t)(HANDLE, PBOOL);

    HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");

    IsWow64Process_t pIsWow64Process = reinterpret_cast<IsWow64ProcessPtr>(
        GetProcAddress(hKernel32, "IsWow64Process")
    );

    BOOL isWow64 = FALSE;
    if (pIsWow64Process)
    {
        pfnIsWow64Process(::GetCurrentProcess(), &isWow64);
    }
    return isWow64 != FALSE;
#endif // Win64/Win32
}

static wxString getWinOsDescription()
{
    wxString str;
    const OSVERSIONINFOEXW info = getWindowsVersionInfo();
    switch ( info.dwPlatformId )
    {
        case VER_PLATFORM_WIN32_NT:
            switch ( info.dwMajorVersion )
            {
                case 5:
                    switch ( info.dwMinorVersion )
                    {
                        case 2:
                            // we can't distinguish between XP 64 and 2003
                            // as they both are 5.2, so examine the product
                            // type to resolve this ambiguity
                            if ( isWindowsServer() == 1 )
                            {
                                str = "Windows Server 2003";
                                break;
                            }
                            //else: must be XP, fall through
                        case 1:
                            str = "Windows XP";
                            break;
                    }
                    break;

                case 6:
                    switch ( info.dwMinorVersion )
                    {
                        case 0:
                            str = isWindowsServer() == 1
                                    ? "Windows Server 2008"
                                    : "Windows Vista";
                            break;

                        case 1:
                            str = isWindowsServer() == 1
                                    ? "Windows Server 2008 R2"
                                    : "Windows 7";
                            break;

                        case 2:
                            str = isWindowsServer() == 1
                                    ? "Windows Server 2012"
                                    : "Windows 8";
                            break;

                        case 3:
                            str = isWindowsServer() == 1
                                    ? "Windows Server 2012 R2"
                                    : "Windows 8.1";
                            break;
                    }
                    break;

                case 10:
                    if ( isWindowsServer() == 1 )
                    {
                        switch ( info.dwBuildNumber )
                        {
                            case WINDOWS_SERVER2016_BUILD:
                                str = "Windows Server 2016";
                                break;
                            case WINDOWS_SERVER2019_BUILD:
                                str = "Windows Server 2019";
                                break;
                            case WINDOWS_SERVER2022_BUILD:
                                str = "Windows Server 2022";
                                break;
                            case WINDOWS_SERVER2025_BUILD:
                                str = "Windows Server 2025";
                                break;
                        }
                    }
                    else
                    {
                        str = info.dwBuildNumber >= FIRST_WINDOWS11_BUILD
                            ? "Windows 11"
                            : "Windows 10";
                    }
                    break;
            }

            if ( str.empty() )
            {
                str.Printf("Windows %s%lu.%lu",
                            isWindowsServer() == 1 ? "Server " : "",
                            info.dwMajorVersion,
                            info.dwMinorVersion);
            }

            str << wxT(" (")
                << wxString::Format("build %lu", info.dwBuildNumber);
            if ( !wxIsEmpty(info.szCSDVersion) )
            {
                str << wxT(", ") << info.szCSDVersion;
            }
            str << wxT(')');

            if ( isPlatform64Bit() )
                str << wxT(", 64-bit edition");
            break;
    }

    return str;
}
}
#endif // __WINDOWS__

wxDECLARE_EVENT(EVT_UPDATE_RESULT, wxCommandEvent);

wxDEFINE_EVENT(EVT_UPDATE_RESULT, wxCommandEvent);

static wxString NA_STR = _L("N/A");

NetworkTestDialog::NetworkTestDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
    : DPIDialog(parent,wxID_ANY,from_u8((boost::format(_utf8(L("Network Test")))).str()),wxDefaultPosition,
            wxSize(1000, 700),
            /*wxCAPTION*/wxDEFAULT_DIALOG_STYLE|wxMAXIMIZE_BOX|wxMINIMIZE_BOX|wxRESIZE_BORDER)
{
    this->SetBackgroundColour(wxColour(255, 255, 255));

	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxBoxSizer* main_sizer;
	main_sizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* top_sizer = create_top_sizer(this);
	main_sizer->Add(top_sizer, 0, wxEXPAND, 5);

	wxBoxSizer* info_sizer = create_info_sizer(this);
	main_sizer->Add(info_sizer, 0, wxEXPAND, 5);

	wxBoxSizer* content_sizer = create_content_sizer(this);
	main_sizer->Add(content_sizer, 0, wxEXPAND, 5);

	wxBoxSizer* result_sizer = create_result_sizer(this);
	main_sizer->Add(result_sizer, 1, wxEXPAND, 5);

	set_default();

	init_bind();

	this->SetSizer(main_sizer);
	this->Layout();

	this->Centre(wxBOTH);
    wxGetApp().UpdateDlgDarkUI(this);
}

wxBoxSizer* NetworkTestDialog::create_top_sizer(wxWindow* parent)
{
	auto sizer = new wxBoxSizer(wxVERTICAL);

	auto line_sizer = new wxBoxSizer(wxHORIZONTAL);
	btn_start = new Button(this, _L("Start Test Multi-Thread"));
    btn_start->SetStyle(ButtonStyle::Confirm, ButtonType::Window);
	line_sizer->Add(btn_start, 0, wxALL, 5);

	btn_start_sequence = new Button(this, _L("Start Test Single-Thread"));
    btn_start_sequence->SetStyle(ButtonStyle::Regular, ButtonType::Window);

	line_sizer->Add(btn_start_sequence, 0, wxALL, 5);

	btn_download_log = new Button(this, _L("Export Log"));
    btn_download_log->SetStyle(ButtonStyle::Regular, ButtonType::Window);
	line_sizer->Add(btn_download_log, 0, wxALL, 5);
	btn_download_log->Hide();

	btn_start->Bind(wxEVT_BUTTON, [this](wxCommandEvent &evt) {
			start_all_job();
		});
	btn_start_sequence->Bind(wxEVT_BUTTON, [this](wxCommandEvent &evt) {
			start_all_job_sequence();
		});
	sizer->Add(line_sizer, 0, wxEXPAND, 5);
	return sizer;
}

wxBoxSizer* NetworkTestDialog::create_info_sizer(wxWindow* parent)
{
	auto sizer = new wxBoxSizer(wxVERTICAL);

	text_basic_info = new wxStaticText(this, wxID_ANY, _L("Basic Info"), wxDefaultPosition, wxDefaultSize, 0);
	text_basic_info->Wrap(-1);
	sizer->Add(text_basic_info, 0, wxALL, 5);

	wxBoxSizer* version_sizer = new wxBoxSizer(wxHORIZONTAL);
	text_version_title = new wxStaticText(this, wxID_ANY, _L("OrcaSlicer Version:"), wxDefaultPosition, wxDefaultSize, 0);
	text_version_title->Wrap(-1);
	version_sizer->Add(text_version_title, 0, wxALL, 5);

	wxString text_version = get_studio_version();
	text_version_val = new wxStaticText(this, wxID_ANY, text_version, wxDefaultPosition, wxDefaultSize, 0);
	text_version_val->Wrap(-1);
	version_sizer->Add(text_version_val, 0, wxALL, 5);
	sizer->Add(version_sizer, 1, wxEXPAND, 5);

	wxBoxSizer* sys_sizer = new wxBoxSizer(wxHORIZONTAL);

	txt_sys_info_title = new wxStaticText(this, wxID_ANY, _L("System Version:"), wxDefaultPosition, wxDefaultSize, 0);
	txt_sys_info_title->Wrap(-1);
	sys_sizer->Add(txt_sys_info_title, 0, wxALL, 5);

	txt_sys_info_value = new wxStaticText(this, wxID_ANY, get_os_info(), wxDefaultPosition, wxDefaultSize, 0);
	txt_sys_info_value->Wrap(-1);
	sys_sizer->Add(txt_sys_info_value, 0, wxALL, 5);

	sizer->Add(sys_sizer, 1, wxEXPAND, 5);

	wxBoxSizer* line_sizer = new wxBoxSizer(wxHORIZONTAL);
	txt_dns_info_title = new wxStaticText(this, wxID_ANY, _L("DNS Server:"), wxDefaultPosition, wxDefaultSize, 0);
	txt_dns_info_title->Wrap(-1);
	txt_dns_info_title->Hide();
	line_sizer->Add(txt_dns_info_title, 0, wxALL, 5);

	txt_dns_info_value = new wxStaticText(this, wxID_ANY, get_dns_info(), wxDefaultPosition, wxDefaultSize, 0);
	txt_dns_info_value->Hide();
	line_sizer->Add(txt_dns_info_value, 0, wxALL, 5);
	sizer->Add(line_sizer, 1, wxEXPAND, 5);

	return sizer;
}

wxBoxSizer* NetworkTestDialog::create_content_sizer(wxWindow* parent)
{
	auto sizer = new wxBoxSizer(wxVERTICAL);

	wxFlexGridSizer* grid_sizer;
	grid_sizer = new wxFlexGridSizer(0, 3, 0, 0);
	grid_sizer->SetFlexibleDirection(wxBOTH);
	grid_sizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	btn_link = new Button(this, _L("Test OrcaSlicer (GitHub)"));
    btn_link->SetStyle(ButtonStyle::Regular, ButtonType::Window);
	grid_sizer->Add(btn_link, 0, wxEXPAND | wxALL, 5);

	text_link_title = new wxStaticText(this, wxID_ANY, _L("Test OrcaSlicer (GitHub):"), wxDefaultPosition, wxDefaultSize, 0);
	text_link_title->Wrap(-1);
	grid_sizer->Add(text_link_title, 0, wxALIGN_RIGHT | wxALL | wxALIGN_CENTER_VERTICAL, 5);

	text_link_val = new wxStaticText(this, wxID_ANY, _L("N/A"), wxDefaultPosition, wxDefaultSize, 0);
	text_link_val->Wrap(-1);
	grid_sizer->Add(text_link_val, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

	btn_bing = new Button(this, _L("Test bing.com"));
    btn_bing->SetStyle(ButtonStyle::Regular, ButtonType::Window);
	grid_sizer->Add(btn_bing, 0, wxEXPAND | wxALL, 5);

    text_bing_title = new wxStaticText(this, wxID_ANY, _L("Test bing.com:"), wxDefaultPosition, wxDefaultSize, 0);

	text_bing_title->Wrap(-1);
	grid_sizer->Add(text_bing_title, 0, wxALIGN_RIGHT | wxALL | wxALIGN_CENTER_VERTICAL, 5);

	text_bing_val = new wxStaticText(this, wxID_ANY, _L("N/A"), wxDefaultPosition, wxDefaultSize, 0);
	text_bing_val->Wrap(-1);
	grid_sizer->Add(text_bing_val, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);
	sizer->Add(grid_sizer, 1, wxEXPAND, 5);

	btn_link->Bind(wxEVT_BUTTON, [this](wxCommandEvent& evt) {
		start_test_github_thread();
	});

	btn_bing->Bind(wxEVT_BUTTON, [this](wxCommandEvent& evt) {
		start_test_bing_thread();
	});

	return sizer;
}
wxBoxSizer* NetworkTestDialog::create_result_sizer(wxWindow* parent)
{
	auto sizer = new wxBoxSizer(wxVERTICAL);
	text_result = new wxStaticText(this, wxID_ANY, _L("Log Info"), wxDefaultPosition, wxDefaultSize, 0);
	text_result->Wrap(-1);
	sizer->Add(text_result, 0, wxALL, 5);

	txt_log = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
	sizer->Add(txt_log, 1, wxALL | wxEXPAND, 5);
	return sizer;
}

NetworkTestDialog::~NetworkTestDialog()
{
    ;
}

void NetworkTestDialog::init_bind()
{
	Bind(EVT_UPDATE_RESULT, [this](wxCommandEvent& evt) {
		if (evt.GetInt() == TEST_ORCA_JOB) {
			text_link_val->SetLabelText(evt.GetString());
		} else if (evt.GetInt() == TEST_BING_JOB) {
			text_bing_val->SetLabelText(evt.GetString());
		}

		std::time_t t = std::time(0);
		std::tm* now_time = std::localtime(&t);
		std::stringstream buf;
		buf << std::put_time(now_time, "%a %b %d %H:%M:%S");
		wxString info = wxString::Format("%s:", buf.str()) + evt.GetString() + "\n";
		try {
			txt_log->AppendText(info);
		}
		catch (std::exception& e) {
			BOOST_LOG_TRIVIAL(error) << "Unkown Exception in print_log, exception=" << e.what();
			return;
		}
		catch (...) {
			BOOST_LOG_TRIVIAL(error) << "Unkown Exception in print_log";
			return;
		}
		return;
	});

	Bind(wxEVT_CLOSE_WINDOW, &NetworkTestDialog::on_close, this);
}

wxString NetworkTestDialog::get_os_info()
{
    wxString text_sys_version;
	int major = 0, minor = 0, micro = 0;
	wxGetOsVersion(&major, &minor, &micro);
	std::string os_version = (boost::format("%1%.%2%.%3%") % major % minor % micro).str();
#ifdef __WINDOWS__
    text_sys_version = WindowsUtils::getWinOsDescription();
#else
    text_sys_version = wxGetOsDescription();
#endif
    text_sys_version += wxString::Format(" %d.%d.%d", major, minor, micro);
	return text_sys_version;
}


wxString NetworkTestDialog::get_dns_info()
{
	return NA_STR;
}

void NetworkTestDialog::start_all_job()
{
	start_test_github_thread();
	start_test_bing_thread();
}

void NetworkTestDialog::start_all_job_sequence()
{
	m_sequence_job = new boost::thread([this] {
		update_status(-1, "start_test_sequence");
        start_test_url(TEST_BING_JOB, "Bing", "http://www.bing.com");
        if (m_closing) return;
		start_test_url(TEST_ORCA_JOB, "OrcaSlicer(GitHub)", "https://github.com/OrcaSlicer/OrcaSlicer");
		if (m_closing) return;
		update_status(-1, "end_test_sequence");
	});
}

void NetworkTestDialog::start_test_url(TestJob job, wxString name, wxString url)
{
	m_in_testing[job] = true;
	wxString info = wxString::Format("test %s start...", name);

	update_status(job, info);

	Slic3r::Http http = Slic3r::Http::get(url.ToStdString());
	info = wxString::Format("[test %s]: url=%s", name,url);

    update_status(-1, info);

    int result = -1;
	http.timeout_max(10)
		.on_complete([this, &result](std::string body, unsigned status) {
			try {
				if (status == 200) {
					result = 0;
				}
			}
			catch (...) {
				;
			}
		})
		.on_ip_resolve([this,name,job](std::string ip) {
			wxString ip_report = wxString::Format("test %s ip resolved = %s", name, ip);
			update_status(job, ip_report);
		})
		.on_error([this,name,job](std::string body, std::string error, unsigned int status) {
		wxString info = wxString::Format("status=%u, body=%s, error=%s", status, body, error);
        this->update_status(job, wxString::Format("test %s failed", name));
        this->update_status(-1, info);
	}).perform_sync();
	if (result == 0) {
        update_status(job, wxString::Format("test %s ok", name));
    }
	m_in_testing[job] = false;
}

void NetworkTestDialog::start_test_ping_thread()
{
	test_job[TEST_PING_JOB] = new boost::thread([this] {
		m_in_testing[TEST_PING_JOB] = true;

		m_in_testing[TEST_PING_JOB] = false;
	});
}
void NetworkTestDialog::start_test_github_thread()
{
    if (m_in_testing[TEST_ORCA_JOB])
        return;
    test_job[TEST_ORCA_JOB] = new boost::thread([this] {
        start_test_url(TEST_ORCA_JOB, "OrcaSlicer(GitHub)", "https://github.com/OrcaSlicer/OrcaSlicer");
    });
}
void NetworkTestDialog::start_test_bing_thread()
{
    test_job[TEST_BING_JOB] = new boost::thread([this] {
        start_test_url(TEST_BING_JOB, "Bing", "http://www.bing.com");
    });
}

void NetworkTestDialog::on_close(wxCloseEvent& event)
{
	m_download_cancel = true;
	m_closing = true;
	for (int i = 0; i < TEST_JOB_MAX; i++) {
		if (test_job[i]) {
			test_job[i]->join();
			test_job[i] = nullptr;
		}
	}

	event.Skip();
}


wxString NetworkTestDialog::get_studio_version()
{
	return wxString(SoftFever_VERSION);
}

void NetworkTestDialog::set_default()
{
	for (int i = 0; i < TEST_JOB_MAX; i++) {
		test_job[i] = nullptr;
		m_in_testing[i] = false;
	}

	m_sequence_job = nullptr;

	text_version_val->SetLabelText(get_studio_version());
	txt_sys_info_value->SetLabelText(get_os_info());
	txt_dns_info_value->SetLabelText(get_dns_info());
	text_link_val->SetLabelText(NA_STR);
	text_bing_val->SetLabelText(NA_STR);
	m_download_cancel = false;
	m_closing = false;
}


void NetworkTestDialog::on_dpi_changed(const wxRect &suggested_rect)
{
    ;
}

void NetworkTestDialog::update_status(int job_id, wxString info)
{
	auto evt = new wxCommandEvent(EVT_UPDATE_RESULT, this->GetId());
	evt->SetString(info);
	evt->SetInt(job_id);
	wxQueueEvent(this, evt);
}


} // namespace GUI
} // namespace Slic3r


