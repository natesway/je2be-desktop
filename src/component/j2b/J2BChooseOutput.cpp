#include "component/j2b/J2BChooseOutput.h"
#include "CommandID.h"
#include "Constants.h"
#include "GameDirectory.h"
#include "component/MainWindow.h"
#include "component/TextButton.h"

using namespace juce;

namespace je2be::gui::component::j2b {

static File DecideDefaultOutputDirectory(J2BConvertState const &s, File root) {
  String name = s.fConfigState.fInputState.fInputDirectory->getFileName();
  File candidate = root.getChildFile(name);
  int count = 0;
  while (candidate.exists()) {
    count++;
    candidate = root.getChildFile(name + "-" + String(count));
  }
  return candidate;
}

File J2BChooseOutput::sLastCustomDirectory;
File J2BChooseOutput::sLastZipFile;

J2BChooseOutput::J2BChooseOutput(J2BConvertState const &convertState) : fState(convertState) {
  auto width = kWindowWidth;
  auto height = kWindowHeight;
  setSize(width, height);

  File root = GameDirectory::BedrockSaveDirectory();
  fDefaultSaveDirectory = DecideDefaultOutputDirectory(convertState, root);

  int y = kMargin;
  fMessage.reset(new Label("", TRANS("Conversion completed! Choose how you want to save it")));
  fMessage->setBounds(kMargin, y, width - 2 * kMargin, kButtonBaseHeight);
  addAndMakeVisible(*fMessage);
  y += fMessage->getHeight();

  y += kMargin;
  fSaveToDefaultDirectory.reset(new component::TextButton(TRANS("Save into Minecraft Bedrock save folder")));
  fSaveToDefaultDirectory->setBounds(2 * kMargin, y, width - 4 * kMargin, kButtonBaseHeight);
  fSaveToDefaultDirectory->setEnabled(root.exists());
  fSaveToDefaultDirectory->onClick = [this]() { onSaveToDefaultButtonClicked(); };
  addAndMakeVisible(*fSaveToDefaultDirectory);
  y += fSaveToDefaultDirectory->getHeight();

  y += kMargin;
  fSaveToCustomDirectory.reset(new component::TextButton(TRANS("Save into custom folder")));
  fSaveToCustomDirectory->setBounds(2 * kMargin, y, width - 4 * kMargin, kButtonBaseHeight);
  fSaveToCustomDirectory->onClick = [this]() { onSaveToCustomButtonClicked(); };
  addAndMakeVisible(*fSaveToCustomDirectory);
  y += fSaveToCustomDirectory->getHeight();

  y += kMargin;
  fSaveAsZipFile.reset(new component::TextButton(TRANS("Export as mcworld file")));
  fSaveAsZipFile->setBounds(2 * kMargin, y, width - 4 * kMargin, kButtonBaseHeight);
  fSaveAsZipFile->onClick = [this]() { onSaveAsZipButtonClicked(); };
  addAndMakeVisible(*fSaveAsZipFile);
  y += fSaveAsZipFile->getHeight();

  {
    int w = 160;
    fBackButton.reset(new component::TextButton(TRANS("Back to the beginning")));
    fBackButton->setBounds(kMargin, height - kMargin - kButtonBaseHeight, w, kButtonBaseHeight);
    fBackButton->onClick = [this]() { onBackButtonClicked(); };
    addAndMakeVisible(*fBackButton);
  }

  fBackButton->setExplicitFocusOrder(2);
}

J2BChooseOutput::~J2BChooseOutput() {}

void J2BChooseOutput::onSaveToDefaultButtonClicked() {
  fState.fCopyDestination = fDefaultSaveDirectory;
  fState.fFormat = J2BOutputFormat::Directory;
  JUCEApplication::getInstance()->invoke(gui::toJ2BCopy, true);
}

void J2BChooseOutput::onSaveToCustomButtonClicked() {
  if (sLastCustomDirectory == File()) {
    sLastCustomDirectory = GameDirectory::BedrockSaveDirectory();
  }
  File directory = sLastCustomDirectory;
  if (auto candidate = DecideDefaultOutputDirectory(fState.fConvertState, directory); candidate != File()) {
    directory = candidate;
  }

  MainWindow::sFileChooser.reset(new FileChooser(TRANS("Select an empty folder to save in"), directory, {}, false));
  int flags = FileBrowserComponent::saveMode | FileBrowserComponent::canSelectDirectories;
  MainWindow::sFileChooser->launchAsync(flags, [this](FileChooser const &chooser) { onCustomDestinationDirectorySelected(chooser); });
}

void J2BChooseOutput::onCustomDestinationDirectorySelected(FileChooser const &chooser) {
  File dest = chooser.getResult();
  if (dest == File()) {
    fSaveToCustomDirectory->setToggleState(false, dontSendNotification);
    fState.fCopyDestination = std::nullopt;
    return;
  }
  sLastCustomDirectory = dest;
  RangedDirectoryIterator it(dest, false);
  bool containsSomething = false;
  for (auto const &e : it) {
    containsSomething = true;
    break;
  }
  if (containsSomething) {
    fSaveToCustomDirectory->setToggleState(false, dontSendNotification);
    fState.fCopyDestination = std::nullopt;
    NativeMessageBox::showMessageBoxAsync(AlertWindow::AlertIconType::WarningIcon, TRANS("Error"),
                                          TRANS("There are files and folders in the directory.\rPlease select an "
                                                "empty folder"));
  } else {
    fState.fCopyDestination = dest;
    fState.fFormat = J2BOutputFormat::Directory;
    JUCEApplication::getInstance()->invoke(gui::toJ2BCopy, true);
  }
}

void J2BChooseOutput::onSaveAsZipButtonClicked() {
  File init = sLastZipFile;
  String fileName = fState.fConvertState.fConfigState.fInputState.fInputDirectory->getFileName();
  if (init == File()) {
    init = File(fileName + ".mcworld");
  } else {
    auto parent = init.getParentDirectory();
    init = parent.getNonexistentChildFile(fileName, ".mcworld", true);
  }
  MainWindow::sFileChooser.reset(new FileChooser(TRANS("Choose where to export the file"), init, "*.mcworld", true));
  int flags = FileBrowserComponent::saveMode | FileBrowserComponent::canSelectFiles | FileBrowserComponent::warnAboutOverwriting;
  MainWindow::sFileChooser->launchAsync(flags, [this](FileChooser const &chooser) { onZipDestinationFileSelected(chooser); });
}

void J2BChooseOutput::onZipDestinationFileSelected(FileChooser const &chooser) {
  File dest = chooser.getResult();
  if (dest == File()) {
    fSaveAsZipFile->setToggleState(false, dontSendNotification);
    fState.fCopyDestination = std::nullopt;
    return;
  }
  sLastZipFile = dest;
  fState.fCopyDestination = dest;
  fState.fFormat = J2BOutputFormat::MCWorld;
  JUCEApplication::getInstance()->invoke(gui::toJ2BCopy, true);
}

void J2BChooseOutput::paint(juce::Graphics &g) {}

void J2BChooseOutput::onBackButtonClicked() {
  JUCEApplication::getInstance()->invoke(gui::toJ2BChooseInput, true);
}

} // namespace je2be::gui::component::j2b
