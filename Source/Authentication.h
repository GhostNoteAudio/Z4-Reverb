#pragma once

#include <JuceHeader.h>

static const juce::String HttpValidationEndpoint = "https://services.ghostnoteaudio.uk/v1/validate";
static const char* STATE_REVOKED = "REVOKED";
static const char* STATE_INVALID = "INVALID";
static const char* STATE_VALID = "VALID";
static const char* STATE_FAILED = "FAILED";

class Authentication
{
	static bool isFreePlugin;
	static bool isAuthenticatedCacheSet;
	static bool isAuthenticatedCacheValue;

public:
	static juce::String AuthFileName;
	static juce::String AuthSalt;

	static inline juce::String GetAppdataDir()
	{
		auto userDataDir = juce::File::getSpecialLocation(juce::File::SpecialLocationType::userApplicationDataDirectory);
		auto ghostNoteDir = userDataDir.getFullPathName() + juce::File::getSeparatorString() + juce::String(JucePlugin_Manufacturer);

		juce::File(ghostNoteDir).createDirectory();
		return ghostNoteDir;
	}

	static inline bool GetIsFreeware()
	{
		return isFreePlugin;
	}

	static inline juce::StringArray GetAuthData()
	{
		if (isFreePlugin)
		{
			juce::StringArray arr;
			arr.add("Free Plugin");
			arr.add("Free Plugin");
			return arr;
		}

		auto dir = GetAppdataDir();
		auto authFile = dir + juce::File::getSeparatorString() + AuthFileName;
		auto f = juce::File(authFile);
		if (f.existsAsFile())
		{
			juce::StringArray arr;
			f.readLines(arr);
			return arr;
		}

		return juce::StringArray();
	}

	static inline void RevokeAuthentication()
	{
		auto dir = GetAppdataDir();
		auto authFile = dir + juce::File::getSeparatorString() + AuthFileName;
		auto f = juce::File(authFile);
		if (f.existsAsFile())
		{
			f.deleteFile();
		}
		isAuthenticatedCacheSet = false;
	}

	static inline juce::String GetDeviceId()
	{
		auto addresses = juce::MACAddress::getAllAddresses();
		juce::String str = "";
		juce::StringArray arr;
		for (int i = 0; i < addresses.size(); i++)
			arr.add(addresses[i].toString());

		arr.sort(true);
		for (int i = 0; i < arr.size(); i++)
			str += arr[i];
		
        auto res = juce::SHA256(str.getCharPointer()).toHexString();
		return res.substring(res.length()-16).toUpperCase();
	}

	static inline juce::String OnlineValidation(juce::String email, juce::String key)
	{
		auto deviceId = GetDeviceId();
		auto urlString = HttpValidationEndpoint + "/" + email + "/" + juce::String(JucePlugin_Name) + "/" + key + "/" + deviceId;
		
		juce::URL url(urlString);
		juce::StringPairArray responseHeaders;
		int statusCode = 0;
		juce::String extraHeaders;
		std::unique_ptr<juce::InputStream> inputStream = url.createInputStream(false, nullptr, nullptr, juce::String(), 3000, &responseHeaders, &statusCode);

		if (inputStream == nullptr || statusCode != 200)
			return STATE_FAILED;

		auto response = inputStream->readEntireStreamAsString();

		if (response == STATE_REVOKED)
		{
			RevokeAuthentication();
			response = STATE_INVALID;
		}

		return response;
	}

	static inline bool IsAuthenticated()
	{
		if (isFreePlugin)
			return true;

		if (isAuthenticatedCacheSet)
			return isAuthenticatedCacheValue;

		auto authData = GetAuthData();
		if (authData.size() != 2)
			return false;

		juce::String hashString = authData[0] + "::" + AuthSalt;
		auto mixed = juce::SHA256(hashString.getCharPointer()).toHexString();
		auto expectedKey = mixed.substring(mixed.length() - 16).toUpperCase();
		auto providedKey = authData[1].replace("-", "").toUpperCase();
		
		auto onlineValidationResponse = OnlineValidation(authData[0], providedKey);

		if (onlineValidationResponse == STATE_FAILED) // If we can't reach the service then fail permissively.
			isAuthenticatedCacheValue = (expectedKey == providedKey);
		else
			isAuthenticatedCacheValue = (expectedKey == providedKey) && onlineValidationResponse == STATE_VALID;
		isAuthenticatedCacheSet = true;
		return isAuthenticatedCacheValue;
	}

	static inline void StoreAuthData(juce::String email, juce::String licenseKey)
	{
		if (isFreePlugin)
			return;

		auto dir = GetAppdataDir();
		auto authFile = dir + juce::File::getSeparatorString() + AuthFileName;
		auto f = juce::File(authFile);
		if (f.existsAsFile())
		{
			f.replaceWithText(email + "\n" + licenseKey, false, false, "\r\n");
		}
		else
		{
			f.create();
			f.appendText(email + "\n" + licenseKey, false, false, "\r\n");
		}
		isAuthenticatedCacheSet = false;
	}
};
