#include <Helper/helpers.h>
#include <string>

// -------------------------------------------------
// Assets "Shaders" 
// -------------------------------------------------
std::string Helpers::GetAssetPath(const std::string& relativePath)
{
    static fs::path assetsRoot = FindAssetsRoot();

    fs::path p = assetsRoot / "Assets" / relativePath;

    return p.make_preferred().string();

}

fs::path Helpers::FindAssetsRoot()
{
    fs::path srcPath = fs::path(__FILE__).parent_path();

    fs::path cur = srcPath;
    while (true) {
        if (fs::exists(cur / "Assets")) return cur;
        if (!cur.has_parent_path()) break;
        cur = cur.parent_path();
    }

    // 2) Start from executable directory (runtime)
    try {
        fs::path exe = fs::current_path(); // fallback to cwd

        cur = exe;
        while (true) {
            if (fs::exists(cur / "Assets")) return cur;
            if (!cur.has_parent_path()) break;
            cur = cur.parent_path();
        }
    }
    catch (...) {
        // ignore and fall back
    }

    // 3) as last resort, return the executable/cwd path
    return fs::current_path();
}

// ------------------------------------------------
// Resources mbx models, textures, etc.
// ------------------------------------------------
std::string Helpers::GetResourcesPath(const std::string& relativePath)
{
    static fs::path resourcesRoot = FindResourcesRoot();

    fs::path p = resourcesRoot / "Resources" / relativePath;

    return p.make_preferred().string();
}

fs::path Helpers::FindResourcesRoot()
{
    fs::path srcPath = fs::path(__FILE__).parent_path();

    fs::path cur = srcPath;
    while (true) {
        if (fs::exists(cur / "Resources")) return cur;
        if (!cur.has_parent_path()) break;
        cur = cur.parent_path();
    }

    // 2) Start from executable directory (runtime)
    try {
        fs::path exe = fs::current_path(); // fallback to cwd

        cur = exe;
        while (true) {
            if (fs::exists(cur / "Resources")) return cur;
            if (!cur.has_parent_path()) break;
            cur = cur.parent_path();
        }
    }
    catch (...) {
        // ignore and fall back
    }

    // 3) as last resort, return the executable/cwd path
    return fs::current_path();
}
