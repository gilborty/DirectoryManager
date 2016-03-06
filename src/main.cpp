#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include <chrono>
#include <thread>
#include <ctime>

#include <sys/stat.h>
#include <dirent.h>


std::string databaseDirectory = "database/";
long memoryUsed = 0;


std::vector<std::string> OpenDir( std::string path )
{
    DIR *dir;
    dirent *pDir;
    
    std::vector<std::string> files;
    dir = opendir( path.c_str() );
    while( pDir = readdir( dir ) )
    {
        files.push_back(pDir->d_name);
    }
    return files;
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

    while( true )
    {
        CheckTime();

        std::vector<std::string> files;
        files = OpenDir( argv[1] );
        
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
                memoryUsed = memoryUsed + memorySize;            
             }
        }
        
        std::cout << "Memory in " << databaseDirectory << ": " << memoryUsed; 
        
        SleepFor( 100 );
    
        memoryUsed = 0;
        std::cout << std::endl;
    }
    
    
    return 0;

}

