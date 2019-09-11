#pragma comment(lib, "Strmiids.lib")
#include <dshow.h>
#include <conio.h>
#include <iostream>


enum VideoStage {
	STAGE_PLAY,
	STAGE_PAUSE,
	STAGE_FORWARD,
};
void AfficherMenu();
void main(void)
{
    IGraphBuilder *pGraph = NULL;
    IMediaControl *pControl = NULL;
    IMediaEvent   *pEvent = NULL;
	IMediaSeeking	*pSeek = NULL;

    // Initialize the COM library.
    HRESULT hr = CoInitialize(NULL);
    if (FAILED(hr))
    {
        printf("ERROR - Could not initialize COM library");
        return;
    }

    // Create the filter graph manager and query for interfaces.
    hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, 
                        IID_IGraphBuilder, (void **)&pGraph);
    if (FAILED(hr))
    {
        printf("ERROR - Could not create the Filter Graph Manager.");
        return;
    }

    hr = pGraph->QueryInterface(IID_IMediaControl, (void **)&pControl);
    hr = pGraph->QueryInterface(IID_IMediaEvent, (void **)&pEvent);
	hr = pGraph->QueryInterface(IID_IMediaSeeking, (void **)&pSeek);

    // Build the graph. IMPORTANT: Change this string to a file on your system.
    hr = pGraph->RenderFile(L"C:\\Example.avi", NULL);
    
	VideoStage stage;
	AfficherMenu();
    if (SUCCEEDED(hr))
    {
        // Run the graph.

        hr = pControl->Run();
		stage = STAGE_PLAY;
		
		while (true)
		{
			int choix;
			choix = _getch();
			
			switch (choix) {
			case 'p' :
			case 'P' :
				if (stage == STAGE_PLAY) {
					hr = pControl->Pause();
					stage = STAGE_PAUSE;
				} else if (stage == STAGE_PAUSE) {
					hr = pControl->Run();
					stage = STAGE_PLAY;
				} else if (stage == STAGE_FORWARD) {
					hr = pSeek->SetRate(1.0);
					stage = STAGE_PLAY;
				}
				else {
					hr = pControl->Pause();
					stage = STAGE_PAUSE;
				}
				break;
			case 'a' :
			case 'A':
				if (stage != STAGE_FORWARD) {
					stage = STAGE_FORWARD;
					hr = pSeek->SetRate(2.0);
				} else {
					hr = pSeek->SetRate(1.0);
					stage = STAGE_PLAY;
				}
				break;
			case 'r' :
			case 'R' :
				hr = pSeek->IsFormatSupported(&TIME_FORMAT_FRAME);
				if (hr == S_OK)
				{
					hr = pSeek->SetTimeFormat(&TIME_FORMAT_FRAME);
					if (SUCCEEDED(hr))
					{
						// Seek to frame number 0.
						LONGLONG rtNow = 0;
						hr = pSeek->SetPositions(
							&rtNow, AM_SEEKING_AbsolutePositioning,
							0, AM_SEEKING_NoPositioning);
					}
				}
				break;
			case 'q' :
			case 'Q' :
				goto fin;
				break;
			default:
				break;
			}
				
		}
        if (SUCCEEDED(hr))
        {
            // Wait for completion.
            long evCode;
            pEvent->WaitForCompletion(INFINITE, &evCode);

            // Note: Do not use INFINITE in a real application, because it
            // can block indefinitely.
        }
    }
fin:
    pControl->Release();
    pEvent->Release();
    pGraph->Release();
    CoUninitialize();
}

void AfficherMenu() {
	std::cout << "Menu des commandes" << std::endl;
	std::cout << "P - Lecture/Pause" << std::endl;
	std::cout << "A - Avance Rapide" << std::endl;
	std::cout << "R - Retour au debut" << std::endl;
	std::cout << "Q - Quitter" << std::endl;
}