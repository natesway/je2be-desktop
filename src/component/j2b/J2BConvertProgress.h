#pragma once

#include "CommandID.h"
#include "ComponentState.h"

namespace je2be::gui {
class TaskbarProgress;
}

namespace je2be::gui::component {
class TextButton;
}

namespace je2be::gui::component::j2b {

class J2BConvertProgress : public juce::Component,
                           public J2BConvertStateProvider,
                           public J2BConfigStateProvider,
                           public J2BChooseInputStateProvider {
public:
  explicit J2BConvertProgress(J2BConfigState const &configState);
  ~J2BConvertProgress() override;

  void paint(juce::Graphics &) override;

  J2BConfigState getConfigState() const override {
    return fState.fConfigState;
  }

  J2BConvertState getConvertState() const override {
    return fState;
  }

  J2BChooseInputState getChooseInputState() const override {
    return fState.fConfigState.fInputState;
  }

  void onCancelButtonClicked();

  void onProgressUpdate(int phase, double done, double total);

  class Updater;

private:
  std::unique_ptr<TextButton> fCancelButton;
  J2BConvertState fState;
  std::unique_ptr<juce::Thread> fThread;
  std::shared_ptr<Updater> fUpdater;
  std::unique_ptr<juce::ProgressBar> fConversionProgressBar;
  std::unique_ptr<juce::ProgressBar> fCompactionProgressBar;
  double fConversionProgress = 0;
  double fCompactionProgress = 0;
  std::unique_ptr<juce::Label> fLabel;
  juce::CommandID fCommandWhenFinished = gui::toJ2BChooseOutput;
  bool fFailed = false;
  std::unique_ptr<juce::TextEditor> fErrorMessage;
  std::unique_ptr<TaskbarProgress> fTaskbarProgress;

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(J2BConvertProgress)
};

} // namespace je2be::gui::component::j2b
