pushd Release
pushd Language

del *.exp
del *.pdb
del *.lib

popd Language

del *.pdb
del *.log

pushd Release
IF NOT EXIST vcruntime140.dll (
  copy C:\Windows\SysWOW64\VCRuntime140.dll
)

IF NOT EXIST mfc140u.dll(
  copy C:\Windows\SysWOW64\mfc140u.dll
)
popd Release
pause