#include "J2BChooseInputComponent.h"
#include "AboutComponent.h"
#include "CommandID.h"
#include "Constants.h"
#include "MainWindow.h"

using namespace juce;

namespace je2be::gui {

File ChooseInputComponent::sLastDirectory;

ChooseInputComponent::ChooseInputComponent(std::optional<ChooseInputState> state)
    : fListThread("j2b::gui::ChooseInputComponent") {
  if (state) {
    fState = *state;
  }

  auto width = kWindowWidth;
  auto height = kWindowHeight;
  auto fileListWidth = 280;

  setSize(width, height);
  {
    fMessage.reset(new Label("", TRANS("Select the world you want to convert")));
    fMessage->setBounds(kMargin, kMargin, width - kMargin - fileListWidth - kMargin - kMargin, height - kMargin - kButtonBaseHeight - kMargin - kMargin);
    fMessage->setJustificationType(Justification::topLeft);
    fMessage->setMinimumHorizontalScale(1);
    addAndMakeVisible(*fMessage);
  }
  {
    fAboutButton.reset(new TextButton("About"));
    fAboutButton->setBounds(kMargin, height - kMargin - kButtonBaseHeight, kButtonMinWidth, kButtonBaseHeight);
    fAboutButton->setMouseCursor(MouseCursor::PointingHandCursor);
    fAboutButton->onClick = [this]() { onAboutButtonClicked(); };
    addAndMakeVisible(*fAboutButton);
  }
  {
    fNextButton.reset(new TextButton(TRANS("Next")));
    fNextButton->setBounds(width - kButtonMinWidth - kMargin, height - kButtonBaseHeight - kMargin, kButtonMinWidth, kButtonBaseHeight);
    fNextButton->onClick = [this]() { onNextButtonClicked(); };
    fNextButton->setEnabled(false);
    addAndMakeVisible(*fNextButton);
  }
  {
    auto w = 140;
    fChooseCustomButton.reset(new TextButton(TRANS("Select from other directories")));
    fChooseCustomButton->setBounds(width - kMargin - fileListWidth, height - kButtonBaseHeight - kMargin, w, kButtonBaseHeight);
    fChooseCustomButton->setMouseCursor(MouseCursor::PointingHandCursor);
    fChooseCustomButton->onClick = [this]() { onChooseCustomButtonClicked(); };
    addAndMakeVisible(*fChooseCustomButton);
  }

  fListThread.startThread();
  fList.reset(new DirectoryContentsList(nullptr, fListThread));

  File dir = File::getSpecialLocation(File::userApplicationDataDirectory).getChildFile(".minecraft").getChildFile("saves");
  fList->setDirectory(dir, true, false);
  fList->addChangeListener(this);

  {
    fListComponent.reset(new FileListComponent(*fList));
    fListComponent->setBounds(width - kMargin - fileListWidth, kMargin, fileListWidth, height - 3 * kMargin - kButtonBaseHeight);
    fListComponent->addListener(this);
    addAndMakeVisible(*fListComponent);
  }

  if (state && state->fInputDirectory && state->fInputDirectory->getParentDirectory() == dir) {
    fInitialSelection = state->fInputDirectory;
  }
}

ChooseInputComponent::~ChooseInputComponent() {
  fListComponent.reset();
  fList.reset();
}

void ChooseInputComponent::changeListenerCallback(ChangeBroadcaster *source) {
  if (!source)
    return;
  if (source != fList.get()) {
    return;
  }
  if (!fInitialSelection) {
    return;
  }
  if (fList->contains(*fInitialSelection)) {
    fListComponent->setSelectedFile(*fInitialSelection);
    fInitialSelection = std::nullopt;
    fList->removeChangeListener(this);
  }
}

void ChooseInputComponent::paint(juce::Graphics &g) {}

void ChooseInputComponent::onNextButtonClicked() {
  JUCEApplication::getInstance()->invoke(gui::toConfig, true);
}

void ChooseInputComponent::onChooseCustomButtonClicked() {
  fInitialSelection = std::nullopt;
  fList->removeChangeListener(this);

  int flags = FileBrowserComponent::openMode | FileBrowserComponent::canSelectDirectories;
  MainWindow::sFileChooser.reset(new FileChooser(TRANS("Select save data folder of Minecraft"), sLastDirectory, ""));
  MainWindow::sFileChooser->launchAsync(flags, [this](FileChooser const &chooser) { onCustomDirectorySelected(chooser); });
}

void ChooseInputComponent::onCustomDirectorySelected(juce::FileChooser const &chooser) {
  File result = chooser.getResult();
  if (result == File()) {
    return;
  }
  fState.fInputDirectory = result;
  sLastDirectory = result.getParentDirectory();
  JUCEApplication::getInstance()->invoke(gui::toConfig, true);
}

void ChooseInputComponent::selectionChanged() {
  int num = fListComponent->getNumSelectedFiles();
  if (num == 1) {
    fState.fInputDirectory = fListComponent->getSelectedFile();
  } else {
    fState.fInputDirectory = std::nullopt;
  }
  if (fState.fInputDirectory != std::nullopt) {
    fNextButton->setEnabled(true);
    fNextButton->setMouseCursor(MouseCursor::PointingHandCursor);
  }
  fInitialSelection = std::nullopt;
  fList->removeChangeListener(this);
}

void ChooseInputComponent::fileClicked(const File &file, const MouseEvent &e) {}

void ChooseInputComponent::fileDoubleClicked(const File &file) {
  fState.fInputDirectory = file;
  JUCEApplication::getInstance()->invoke(gui::toConfig, false);
}

void ChooseInputComponent::browserRootChanged(const File &newRoot) {}

void ChooseInputComponent::onAboutButtonClicked() {
  DialogWindow::LaunchOptions options;
  options.content.setOwned(new AboutComponent());
  options.dialogTitle = "About";
  options.useNativeTitleBar = true;
  options.escapeKeyTriggersCloseButton = true;
  options.resizable = false;
  options.dialogBackgroundColour = getLookAndFeel().findColour(ResizableWindow::backgroundColourId);
  options.launchAsync();
}

} // namespace je2be::gui