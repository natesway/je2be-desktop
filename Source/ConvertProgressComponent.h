#pragma once

#include "CommandID.h"
#include "ComponentState.h"

class ConvertProgressComponent : public juce::Component,
                                 public ConvertStateProvider,
                                 public ConfigStateProvider,
                                 public ChooseInputStateProvider {
public:
  explicit ConvertProgressComponent(ConfigState const &configState);
  ~ConvertProgressComponent() override;

  void paint(juce::Graphics &) override;

  ConfigState getConfigState() const override {
    return fState.fConfigState;
  }

  ConvertState getConvertState() const override {
    return fState;
  }

  ChooseInputState getChooseInputState() const override {
    return fState.fConfigState.fInputState;
  }

  void onCancelButtonClicked();

  void onProgressUpdate(int phase, double done, double total);

  class Updater;

private:
  std::unique_ptr<juce::TextButton> fCancelButton;
  ConvertState fState;
  std::unique_ptr<juce::Thread> fThread;
  std::shared_ptr<Updater> fUpdater;
  std::unique_ptr<juce::ProgressBar> fProgressBar;
  double fProgress = 0;
  std::unique_ptr<juce::Label> fLabel;
  juce::CommandID fCommandWhenFinished = gui::toChooseOutput;
  bool fFailed = false;
  std::unique_ptr<juce::TextEditor> fErrorMessage;

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConvertProgressComponent)
};
