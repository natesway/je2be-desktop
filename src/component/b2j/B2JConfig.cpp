#include <je2be.hpp>

#include "CommandID.h"
#include "Constants.h"
#include "GameDirectory.h"
#include "component/TextButton.h"
#include "component/b2j/B2JConfig.h"

using namespace juce;

namespace je2be::desktop::component::b2j {

B2JConfig::B2JConfig(ChooseInputState const &chooseInputState) : fState(chooseInputState) {
  auto width = kWindowWidth;
  auto height = kWindowHeight;
  setSize(width, height);

  int y = kMargin;
  juce::String label = fState.fInputState.fInput.getFullPathName();
  fFileOrDirectory.reset(new Label("", TRANS("Selected world:") + " " + label));
  fFileOrDirectory->setBounds(kMargin, kMargin, width - kMargin * 2, kButtonBaseHeight);
  fFileOrDirectory->setJustificationType(Justification::topLeft);
  addAndMakeVisible(*fFileOrDirectory);

  {
    y += 3 * kMargin;
    fImportAccountFromLauncher.reset(new ToggleButton(TRANS("Import player ID from the Minecraft Launcher to embed it into level.dat as a local player ID")));
    fImportAccountFromLauncher->setBounds(kMargin, y, width - kMargin * 2, kButtonBaseHeight);
    fImportAccountFromLauncher->setMouseCursor(MouseCursor::PointingHandCursor);
    fImportAccountFromLauncher->onClick = [this] {
      onClickImportAccountFromLauncherButton();
    };
    addAndMakeVisible(*fImportAccountFromLauncher);
    y += fImportAccountFromLauncher->getHeight();
  }

  {
    int x = 42;

    juce::String title = TRANS("Account:") + " ";
    fAccountListLabel.reset(new Label("", title));
    int labelWidth = getLookAndFeel().getLabelFont(*fAccountListLabel).getStringWidth(title) + 10;
    fAccountListLabel->setBounds(x, y, labelWidth, kButtonBaseHeight);
    fAccountListLabel->setJustificationType(Justification::centredLeft);
    addAndMakeVisible(*fAccountListLabel);

    fAccountList.reset(new ComboBox());
    fAccountList->setBounds(x + labelWidth, y, width - x - labelWidth - kMargin, kButtonBaseHeight);
    fAccountList->setEnabled(false);
    addAndMakeVisible(*fAccountList);
    y += fAccountList->getHeight();
  }

  int messageComponentY = y + kMargin;

  fStartButton.reset(new component::TextButton(TRANS("Start")));
  fStartButton->setBounds(width - kMargin - kButtonMinWidth, height - kMargin - kButtonBaseHeight, kButtonMinWidth, kButtonBaseHeight);
  fStartButton->setEnabled(false);
  fStartButton->onClick = [this]() { onStartButtonClicked(); };
  addAndMakeVisible(*fStartButton);

  fBackButton.reset(new component::TextButton(TRANS("Back")));
  fBackButton->setBounds(kMargin, height - kMargin - kButtonBaseHeight, kButtonMinWidth, kButtonBaseHeight);
  fBackButton->onClick = [this]() { onBackButtonClicked(); };
  addAndMakeVisible(*fBackButton);

  // TODO: check given file or directory
  fOk = true;

  if (fOk) {
    fMessage.reset(new Label("", ""));
  } else {
    fMessage.reset(new Label("", TRANS("There doesn't seem to be any Minecraft save data in the specified folder.")));
    fMessage->setColour(Label::textColourId, kErrorTextColor);
  }
  fMessage->setBounds(kMargin, messageComponentY, width - 2 * kMargin, kButtonBaseHeight);
  addAndMakeVisible(*fMessage);

  fImportAccountFromLauncher->setEnabled(false);
  fStartButton->setEnabled(false);
  fBackButton->setEnabled(false);
  fAccountScanThread.reset(new AccountScanThread(this));
  fAccountScanThread->startThread();

  startTimer(1000);
}

B2JConfig::~B2JConfig() {}

void B2JConfig::timerCallback() {
  stopTimer();
  updateStartButton();
}

void B2JConfig::updateStartButton() {
  if (fAccountScanThread == nullptr && !isTimerRunning()) {
    fStartButton->setEnabled(fOk);
  } else {
    fStartButton->setEnabled(false);
  }
}

void B2JConfig::paint(juce::Graphics &g) {}

void B2JConfig::onStartButtonClicked() {
  if (fImportAccountFromLauncher->getToggleState()) {
    int selected = fAccountList->getSelectedId();
    int index = selected - 1;
    if (0 <= index && index < fAccounts.size()) {
      Account a = fAccounts[index];
      fState.fLocalPlayer = a.fUuid;
    }
  }
  JUCEApplication::getInstance()->invoke(commands::toB2JConvert, true);
}

void B2JConfig::onBackButtonClicked() {
  JUCEApplication::getInstance()->invoke(commands::toChooseBedrockInput, true);
}

void B2JConfig::onClickImportAccountFromLauncherButton() {
  if (fAccountScanThread) {
    return;
  }
  fAccountList->setEnabled(fImportAccountFromLauncher->getToggleState());
}

void B2JConfig::handleAsyncUpdate() {
  if (fAccountScanThread) {
    fAccountScanThread->copyAccounts(fAccounts);
  }
  fAccountScanThread.reset();
  fAccountList->clear();
  for (int i = 0; i < fAccounts.size(); i++) {
    Account account = fAccounts[i];
    fAccountList->addItem(account.toString(), i + 1);
  }
  if (fAccounts.size() > 0) {
    fAccountList->setSelectedId(1);
  }
  fAccountList->setEnabled(true);
  fImportAccountFromLauncher->setEnabled(true);
  fImportAccountFromLauncher->setToggleState(fAccounts.size() > 0, dontSendNotification);
  updateStartButton();
  fBackButton->setEnabled(true);
}

} // namespace je2be::desktop::component::b2j
