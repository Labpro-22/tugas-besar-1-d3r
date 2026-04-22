#pragma once

#include <string>
#include <map>
#include <memory>

class AssetManager {
private:
    std::map<std::string, std::shared_ptr<void>> assets;
    std::string assetPath;

    AssetManager();
    static AssetManager* instance;

public:
    static AssetManager* getInstance();
    
    void setAssetPath(const std::string& path);
    
    // Load assets
    bool loadImage(const std::string& key, const std::string& filepath);
    bool loadFont(const std::string& key, const std::string& filepath, int size);
    bool loadSound(const std::string& key, const std::string& filepath);
    
    // Get assets
    void* getImage(const std::string& key);
    void* getFont(const std::string& key);
    void* getSound(const std::string& key);
    
    // Unload
    void unloadAsset(const std::string& key);
    void unloadAll();
    
    ~AssetManager();
};
