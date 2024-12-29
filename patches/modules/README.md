# Patch Modules

Put your patchs related to modules in this folder.
All the magic is ruled by patch_modules.py in scripts folder.

To apply modules patches, run the following command with one argument describing the module path you want target
Example: lib/hostap

```
west patch-modules --module-name lib/hostap
```

You can't patch more than one module.