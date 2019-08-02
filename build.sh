mkdir .build
cd .build
cmake -G "Visual Studio 15 Win64" -DBoost_DEBUG=ON -DFLEXBIN_ENABLE_TEST=1  -Dgtest_force_shared_crt=ON .. 
cmake --build . 2>&1 > build.log
cd ..
