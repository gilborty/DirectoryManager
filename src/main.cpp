#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include <chrono>
#include <thread>
#include <unordered_map>
#include <ctime>

#include <sys/stat.h>
#include <dirent.h>


std::string databaseDirectory = "database/";
long memoryUsed = 0;
long memoryLimit = 300000000;

long secondsUntilDelete = 15;

std::unordered_map<std::string, time_t> filesToBeDeleted;

std::vector<std::string> OpenDir( std::string path )
{
    DIR *dir;
    dirent *pDir;

    std::vector<std::string> files;

    dir = opendir( path.c_str() );
    if( dir == NULL )
    {
        std::cout << "Cannot open directory: " << path << std::endl;
        std::vector<std::string> nullString;
        return nullString;
    }

    while( (pDir = readdir( dir )) != NULL )
    {
        files.push_back(pDir->d_name);
    }

    closedir(dir);
    return files;

}

void AddToMap( std::pair<std::string, time_t> pairIn )
{
    for( auto value : filesToBeDeleted )
    {

        if( pairIn.first.compare( value.first ) )
        {
            //Values are not equal
            //Keep looking
        }
        else
        {
            //This value is already in the map
            //Break out of the for loop
            return;
        }
    }
    //std::cout << "Adding file: " << pairIn.first << std::endl;
    filesToBeDeleted.insert( pairIn );
}

void ClearMemory()
{
        std::cout << "Clear Memory called." << std::endl;
        std::vector<std::string> filesToRemoveFromMap;

        auto timeNow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

        auto compareTime = timeNow - secondsUntilDelete;
        std::cout << "Files older than: " << std::ctime(&compareTime) << std::endl;
    for( auto file : filesToBeDeleted )
    {
        //Check if file is older than specified time
        if( file.second < compareTime )
        {
            //std::cout << file.first << " is old" << std::endl;
            std::string fullPath = databaseDirectory + file.first;
            remove( fullPath.c_str() );

            //Set remove it from the map
            filesToRemoveFromMap.push_back(file.first);
        }
    }

    //Remove values from map
    for( auto value : filesToRemoveFromMap )
    {
        filesToBeDeleted.erase( value );
    }
}

void SleepFor( int milliseconds )
{
    std::this_thread::sleep_for( std::chrono::milliseconds( milliseconds ) );
}

void CheckTime()
{
    std::chrono::time_point<std::chrono::system_clock> currentTime;
    currentTime = std::chrono::system_clock::now();
    std::time_t reportTime = std::chrono::system_clock::to_time_t(currentTime);
    std::cout << "The time is: " << std::ctime( &reportTime ) << std::endl;
}


int main( int argc, char* argv[] )
{
    auto startTime = std::chrono::system_clock::now();

    while( true )
    {
        //CheckTime();

        std::vector<std::string> files;
        files = OpenDir( databaseDirectory.c_str() );

        for( auto& element : files )
        {
            //Check for top level and current level directories
            //If they are files we want to look at, proceed
            if( element.compare( 0, 1, "." ) != 0 )
            {

                std::string fullPath = databaseDirectory + element;

                struct stat buffer;
                stat( fullPath.c_str(), &buffer );

                int memorySize = buffer.st_size;
                time_t timeAccessed = buffer.st_ctime;

                auto pair = std::make_pair(element, timeAccessed);
                AddToMap( pair );

                memoryUsed = memoryUsed + memorySize;
             }
        }

        std::cout << "Memory in " << databaseDirectory << ": " << memoryUsed/1000000 << " MB" << std::endl;

        if( memoryUsed > memoryLimit )
        {
            std::cout << "MEMORY LIMIT EXCEEDED!" << std::endl;
            ClearMemory();
        }

        SleepFor( 100 );

        memoryUsed = 0;
        auto nowTime = std::chrono::system_clock::now();
        std::chrono::duration<double> elapsedSeconds = nowTime - startTime;
        std::cout << "Uptime: " << elapsedSeconds.count() << std::endl;
        std::cout << std::endl;
    }


    return 0;

}
