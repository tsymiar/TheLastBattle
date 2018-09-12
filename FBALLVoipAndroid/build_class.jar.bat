cd bin/classes
jar -cvf class.jar *
cd ..
cd ..
copy /y bin\classes\class.jar ..\..\Client\Assets\Plugins\Android\bin
@pause