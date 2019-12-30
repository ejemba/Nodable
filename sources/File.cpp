#include "File.h"
#include "FileView.h"
#include "ContainerView.h"
#include "Container.h"
#include "Lexer.h"
#include "View.h"
#include "Variable.h"
#include "Log.h"

#include "IconFontCppHeaders/IconsFontAwesome5.h"

#include <Windows.h>
#include <SDL.h>
#include <SDL2\include\SDL_syswm.h>
#include <fstream>

using namespace Nodable;

void File::save() const
{
	std::ofstream fileStream(this->path.c_str());
	fileStream.write(content.c_str(), content.size());	
}

void File::setContent(std::string& _content)
{
	content = _content;
}

File* File::CreateFileWithPath(const char* _filePath)
{
	/*
		Creates the File
	*/

	// Sanitize path to get only slashes, and no antislashes
	std::string cleanedFilePath(_filePath);
	std::replace(cleanedFilePath.begin(), cleanedFilePath.end(), '/', '\\');

	// Extract file name from filePath
	std::string name = cleanedFilePath;
	auto firstSlashPosition = cleanedFilePath.find_last_of('\\');
	if (firstSlashPosition != std::string::npos)
		name = cleanedFilePath.substr(firstSlashPosition + 1, cleanedFilePath.size() - firstSlashPosition - 1);

	std::ifstream fileStream(cleanedFilePath.c_str());

	if (!fileStream.is_open())
	{
		LOG_MSG("Unable to load \"%s\"\n", cleanedFilePath.c_str());
		return nullptr;
	}

	LOG_MSG("Loading \"%s\"\n", cleanedFilePath.c_str());
	std::string content((std::istreambuf_iterator<char>(fileStream)), std::istreambuf_iterator<char>());
	File* file = new File(cleanedFilePath.c_str(), content.c_str(), name.c_str());

	/*
		Creates the FileView
	*/

	auto fileView = new FileView();
	file->addComponent("view", fileView);
	fileView->init();
	fileView->setTextEditorContent(file->getText());

	// Add a container to the application to contain all nodes :
	auto container = new Container;
	file->addComponent("container", container);
	container->addComponent("view", new ContainerView);
	container->setOwner(file);

	return file;
}

std::string File::BrowseForFileAndReturnItsAbsolutePath(SDL_Window* currentWindow)
{
	OPENFILENAME ofn;                             // common dialog box structure
	char szFile[512];                             // buffer for file name            
	HANDLE hf;                                    // file handle

	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(currentWindow, &wmInfo);
	HWND hwnd = wmInfo.info.win.window;           // owner sdlWindow

	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = szFile;

	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open dialog box. 

	if (GetOpenFileName(&ofn))
		return szFile;

	return "";
}

bool File::evaluateExpression(std::string& _expression)
{
	LOG_MSG("Application::eval() - create a variable.\n");

	auto var = getContainer()->createNodeVariable(ICON_FA_CODE);
	reinterpret_cast<View*>( var->getComponent("view"))->setVisible(false);

	LOG_DBG("Lexer::eval() - assign the expression string to that variable\n");
	var->setValue(_expression);

	LOG_DBG("Lexer::eval() - check if expression is not empty\n");
	if (var->isSet())
	{
		/* Create a Lexer node. The lexer will cut expression string into tokens
		(ex: "2*3" will be tokenized as : number"->"2", "operator"->"*", "number"->"3")*/
		LOG_DBG("Lexer::eval() - create a lexer with the expression string\n");
		auto lexer = getContainer()->createNodeLexer(var);
		return lexer->eval();
		//container->destroyNode(lexer);
	}

	return false;
}

bool File::update() {

	auto hasChanged = getContainer()->update();
	
	if (!hasChanged)
		return false;

	auto result		= getContainer()->find("result");
	auto member		= result->getValueMember();
	auto expression = member->getSourceExpression();
	auto view		= getComponent("view")->getAs<FileView*>();

	view->replaceHighlightedPortionInTextEditor(expression);
	
}

bool File::clearContextAndEvalHighlightedExpression()
{
	bool success;

	getContainer()->clear();

	auto view = getComponent("view")->getAs<FileView*>();

	auto expression = view->getTextEditorHighlightedExpression();
	success = evaluateExpression(expression);

	return success;
}