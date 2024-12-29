# Patch Zephyr

Put your patchs related to zephyr repository in this folder.
All the magic is ruled by patch_zephyr.py in scripts folder.

To apply zephyr patches, run the following command:

```
west patch-zephyr
```

# How to generate a patch

Go to where you need to modify sources. Update your sources, commit them.

To generate a patch of the last commit, use:

```
git format-patch -1 HEAD
```

this will generate a file in the same directory:

Ex: 0001-add-eusart-spi-driver.patch
