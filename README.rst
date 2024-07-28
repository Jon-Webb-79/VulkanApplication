******************
Vulkan Application
******************
This is a simple application that provides an object oriented implementation
of the `Vulkan Tutorial <https://vulkan-tutorial.com/>`_ up to the point of indexed indices.

Contributing
############
This package is archived for my internal purposes.  However, if you wish to 
contribute, please contact the author to authorize a pull request

License
#######
This software is developed under a simple MIT license.

Requirements
############
This library is developed and tested on Macintosh and Arch Linux Operating
Systems.  It is developed with ``g++ 14.1.1`` and ``clang 16.0.6`` compilers. In
addition, this code base requires the use of ``CMake 3.29.6``, and 
``valgrind``.

Installation
############
This project is covered under a basic MIT license which allows anyone to use 
this code base or to contribute to it with the express permsission of the 
git project owner.

Use Code Base 
-------------
In order to download this repository from github, follow these instructions

#. Ensure you have ``.git`` installed on your computer

#. Ensure you have ``cmake`` installed on your computer.  This code-base requires 
   cmake version 3.27.7 or later versions.

#. Download this repository to your preferred directory with the following command;

   .. code-block:: bash 

      git clone https://github.com/Jon-Webb-79/VulkanApplication.git VulkanApplication 

#. Navigate to either the bash or zshell scripts directory depending on your 
   environment with one of the following commands.

   .. code-block:: bash 

      cd csalt/scripts/bash 
      cd csalt/scripts/zsh 

#. Build the code base with the following command.

   .. code-block:: bash 

      # If using bash
      bash debug.sh  
      # If using zsh 
      zsh debug.zsh

#. Navigate to ``csalt/csalt/build/debug`` to run unit tests 

   .. code-block:: bash 

      valgrind ./VulkanApplication
      
#. If all unit tests do not pass with no memory leaks, you may need to contact 
   the administrator of this git repository.  If they do pass, then you are set 
   to transform this code-base into a static or dynamic library, or just 
   copy the ``.c`` and ``.h`` files to your project.

