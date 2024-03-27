#ifndef BCOMP_LOADER_H
#define BCOMP_LOADER_H
#include <string>
#include <vector>

class T_betree;
class MessageHandler;

/**
 * Helper class allowing loading betrees with the B compiler. This class
 * simplifies using .db files, or search paths.
 *
 * The error messages are displayed through a MessageHandler object to simplify
 * customising the error message to different environments, or integrating the
 * messages in a GUI.
 */
class BCOMPLoader
{
public:
    /**
     * Create a new BCOMPLoader object using the given handler for displaying
     * messages.
     * If handler h is NULL, a handler displaying messages to stdout is created and
     * used.
     *
     * @param h the handler used to display messages, or NULL to use a default handler
     */
    BCOMPLoader(MessageHandler *h = NULL);
    ~BCOMPLoader();

    /**
     * Loads the given resource file
     *
     * @param file the resource file to load
     */
    void loadResourceFile(const std::string& file);

    /**
     * Adds the given path to the search path for searching components
     */
    void setSearchPath(const std::vector<std::string>& path);
    /**
     * Loads the given .db file. If a .db file is used, search pathes will be ignored
     *
     * @param file the path to the project .db file
     */
    void loadDbFile(const std::string& file);


    /**
     * Loads the given component.
     *
     * @param comp the file name or the name of the component
     * @param semantic indicate whether the semantic analysis should be performed
     * @param b0 indicate whether the component should be B0 checked
     * @return the betree corresponding to the component, or NULL if an error
     *   occured.
     */
    T_betree *loadComponent(const std::string& comp, const std::string& converterName, bool semantic = true, bool b0 = true);

    /**
     * Unload a component that has been loaded with loadComponent.
     *
     * @param component the component that is unloaded
     */
    void unloadComponent(T_betree *component);

    /**
     * @brief isEventB
     * @return true if the project associated to db file is of SYSTEM type
     */
    bool isEventB();

private:
    bool handleBCOMPErrors();
    void loadLibraries(const std::string& bdp_path, const std::string& name);
    void loadLibrary(const std::string& bdp_path, const std::string& name);
    void readAllStrings(const std::string&file_name,
                        const char *separators,
                        std::vector<std::string>& result);
    std::string readUntil(std::ifstream &input, const char *expected) const;

    MessageHandler *handler;
    bool hasDb;
    bool deleteHandler;
    bool eventB;
};

#endif // BCOMP_LOADER_H
