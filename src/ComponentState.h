#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include <optional>
#include <unordered_map>

namespace je2be::desktop {

enum class InputType {
  Java,
  Bedrock,
  Xbox360,
};

class ChooseInputState {
public:
  ChooseInputState(InputType type, juce::File input, juce::String worldName) : fType(type), fInput(input), fWorldName(worldName) {}

  InputType fType;
  juce::File fInput;
  juce::String fWorldName;
};

class ChooseInputStateProvider {
public:
  virtual ~ChooseInputStateProvider() {}
  virtual std::optional<ChooseInputState> getChooseInputState() const = 0;
};

class J2BConfigState {
public:
  explicit J2BConfigState(ChooseInputState const &inputState)
      : fInputState(inputState) {}
  ChooseInputState const fInputState;
  enum class DirectoryStructure {
    Vanilla,
    Paper,
  };
  DirectoryStructure fStructure = DirectoryStructure::Vanilla;
};

class J2BConfigStateProvider {
public:
  virtual ~J2BConfigStateProvider() {}
  virtual J2BConfigState getConfigState() const = 0;
};

class BedrockConvertedState {
public:
  BedrockConvertedState(juce::String const &worldName, juce::File const &outputDirectory) : fWorldName(worldName), fOutputDirectory(outputDirectory) {}
  juce::String fWorldName;
  juce::File fOutputDirectory;
};

class BedrockConvertedStateProvider {
public:
  virtual ~BedrockConvertedStateProvider() {}
  virtual std::optional<BedrockConvertedState> getConvertedState() const = 0;
};

enum class BedrockOutputFormat {
  Directory,
  MCWorld,
};

class BedrockOutputChoosenState {
public:
  explicit BedrockOutputChoosenState(BedrockConvertedState const &convertedState)
      : fConvertedState(convertedState), fFormat(BedrockOutputFormat::Directory) {}

  BedrockConvertedState const fConvertedState;
  BedrockOutputFormat fFormat;
  std::optional<juce::File> fCopyDestination;
};

class BedrockOutputChoosenStateProvider {
public:
  virtual ~BedrockOutputChoosenStateProvider() {}
  virtual BedrockOutputChoosenState getBedrockOutputChoosenState() const = 0;
};

class B2JConfigState {
public:
  explicit B2JConfigState(ChooseInputState const &inputState)
      : fInputState(inputState) {}
  ChooseInputState const fInputState;
  std::optional<juce::Uuid> fLocalPlayer;
};

class B2JConfigStateProvider {
public:
  virtual ~B2JConfigStateProvider() {}
  virtual B2JConfigState getConfigState() const = 0;
};

class JavaConvertedState {
public:
  JavaConvertedState(juce::String const &worldName, juce::File const &outputDirectory) : fWorldName(worldName), fOutputDirectory(outputDirectory) {}
  juce::String fWorldName;
  juce::File fOutputDirectory;
};

class JavaConvertedStateProvider {
public:
  virtual ~JavaConvertedStateProvider() {}
  virtual std::optional<JavaConvertedState> getConvertedState() const = 0;
};

class JavaOutputChoosenState {
public:
  explicit JavaOutputChoosenState(JavaConvertedState const &convertedState)
      : fConvertedState(convertedState) {}

  JavaConvertedState const fConvertedState;
  std::optional<juce::File> fCopyDestination;
};

class JavaOutputChoosenStateProvider {
public:
  virtual ~JavaOutputChoosenStateProvider() {}
  virtual JavaOutputChoosenState getJavaOutputChoosenState() const = 0;
};

class X2JConfigState {
public:
  explicit X2JConfigState(ChooseInputState const &inputState)
      : fInputState(inputState) {}
  ChooseInputState const fInputState;
  std::optional<juce::Uuid> fLocalPlayer;
};

class X2JConfigStateProvider {
public:
  virtual ~X2JConfigStateProvider() {}
  virtual X2JConfigState getConfigState() const = 0;
};

class X2BConfigState {
public:
  explicit X2BConfigState(ChooseInputState const &inputState)
      : fInputState(inputState) {}
  ChooseInputState const fInputState;
};

class X2BConfigStateProvider {
public:
  virtual ~X2BConfigStateProvider() {}
  virtual X2BConfigState getConfigState() const = 0;
};

} // namespace je2be::desktop
