# BuckleScript bindings to @react-native-community/image-editor

[![Build Status](https://github.com/reason-react-native/image-editor/workflows/Build/badge.svg)](https://github.com/reason-react-native/image-editor/actions)
[![Version](https://img.shields.io/npm/v/@reason-react-native/image-editor.svg)](https://www.npmjs.com/package/@reason-react-native/image-editor)

These are complete BuckleScript bindings to
[`@reason-react-native/image-editor`](https://github.com/react-native-community/react-native-image-editor),
in Reason syntax.

Version `x.y.z` of `@reason-react-native/image-editor` should be compatible with
version `x.y.*` of `@react-native-community/image-editor`.

## Installation

With `yarn`:

```shell
yarn add @reason-react-native/image-editor
```

With `npm`:

```shell
npm install @reason-react-native/image-editor
```

`@react-native-community/image-editor` should be properly installed and linked.
Please refer to the relevant
[instructions](https://github.com/react-native-community/react-native-image-editor/blob/master/README.md).

`@reason-react-native/image-editor` should be added to `bs-dependencies` in your
`bsconfig.json`. Something like

```diff
{
  //...
  "bs-dependencies": [
    "reason-react",
    "reason-react-native",
    // ...
+    "@reason-react-native/image-editor"
  ],
  //...
}
```

## Methods

### `cropImage`

`cropImage` takes arguments of type `source` and `cropData` and, if the image is
successfully cropped, returns path of the resulting image as a `string`, wrapped
in a Promise. If a remote image cannot be downloaded or an image cannot be
cropped, the Promise will be rejected.

```reason
cropImage: (source, cropData) => Js.Promise.t(string)
```

### `fromRequired`

To convert a `ReactNative.Packager.required` object into [`source`](#source).

```reason
fromRequired: ReactNative.Packager.required => source
```

### `fromUriSource`

To convert a URI given as a `string` into [`source`](#source).

```reason
fromUriSource: string => source
```

## Types

### `source`

An abstract type created using the [`fromRequired`](#fromrequired) and
[`fromUriSource`](#fromurisource) methods.

### `offset`

An abstract type created using the constructor of the same name which takes
named arguments `x` and `y` of type `int`.

```reason
offset: (~x: int, ~y: int) => offset
```

### `size`

An abstract type created using the constructor of the same name which takes
named arguments `width` and `height` of type `int`.

```reason
size: (~width: int, ~height: int) => size
```

### `cropData`

An abstract type created using the constructor of the same name which takes
named arguments `offset` (of type `offset`) and `size` (of type `size`) and
optional arguments `displaySize` (of type `size`) and `resizeMode` (one of
polymorphic variants `` `contain ``, `` `cover ``, `` `stretch ``).

```reason
cropData: (
  ~offset: offset,
  ~size: size,
  ~displaySize: size=?,
  ~resizeMode=[ | `contain | `cover | `stretch]=?,
  unit
) => cropData
```

## Example

```reason
open ReactNative;

// hardcoding actual image dimensions
let imageWidth = 3396.;
let imageHeight = 2388.;

let windowWidth = Dimensions.get(`window)##width;
let windowHeight = Dimensions.get(`window)##height;

let displayWidth = windowWidth *. 0.9;
let displayHeight = windowWidth *. 0.9 *. imageHeight /. imageWidth;

let styles =
  Style.(
    StyleSheet.create({
      "container":
        style(
          ~width=windowWidth->dp,
          ~height=windowHeight->dp,
          ~flexDirection=`column,
          ~alignItems=`center,
          ~justifyContent=`center,
          (),
        ),
      "frame":
        style(
          ~width=displayWidth->dp,
          ~height=displayHeight->dp,
          ~alignItems=`center,
          ~justifyContent=`center,
          ~borderWidth=StyleSheet.hairlineWidth,
          (),
        ),
    })
  );

type state = {
  path: option(string),
  imageLoaded: bool,
};

type action =
  | SetPath(option(string))
  | SetImageLoaded;

let imageUri = "https://images.unsplash.com/photo-1520453803296-c39eabe2dab4";
let uri = ReactNativeImageEditor.fromUriSource(imageUri);

let handleCropImage = (cropData, send, handler) =>
	Js.Promise.(
		ReactNativeImageEditor.cropImage(uri, cropData)
		|> then_(successURI => resolve(send(handler(successURI))))
    |> catch(err => resolve(Js.Console.warn(err)))
    |> ignore
  );

[@react.component]
let make = () => {
  let (state, dispatch) =
    React.useReducer(
      (state, action) =>
        switch (action) {
        | SetPath(p) => {...state, path: p}
        | SetImageLoaded => {...state, imageLoaded: true}
        },
      {path: None, imageLoaded: false},
    );

  let size =
    ReactNativeImageEditor.size(
      ~width=(imageWidth *. 0.5)->floor->truncate,
      ~height=(imageHeight *. 0.5)->floor->truncate,
    );

  let cropData = offset =>
    ReactNativeImageEditor.cropData(
      ~offset,
      ~size,
      ~resizeMode=`cover,
      (),
    );

  <View style=styles##container>
      <Text>
        "Click on a quadrant of the image to crop it."->React.string
      </Text>
      <View style=styles##frame>
        <TouchableOpacity
          onPress={e =>
            handleCropImage(
              ReactNativeImageEditor.offset(
                ~x=
                  {e##nativeEvent##locationX /. displayWidth < 0.5
                     ? 0 : (0.5 *. imageWidth)->floor->truncate},
                ~y=
                  {e##nativeEvent##locationY /. displayHeight < 0.5
                     ? 0 : (0.5 *. imageHeight)->floor->truncate},
              )
              ->cropData,
              dispatch,
              link =>
              SetPath(Some(link))
            )
          }>
          <Image
            source={Image.Source.fromUriSource(
              Image.uriSource(
                ~uri=
                  Belt.Option.getWithDefault(
                    state.path,
                    imageUri,
                  ),
                ~width=displayWidth,
                ~height=
                  if (state.imageLoaded) {
                    displayHeight;
                  } else {
                    0.;
                  },
                (),
              ),
            )}
            resizeMode=`contain
            onLoadEnd={() => dispatch(SetImageLoaded)}
          />
        </TouchableOpacity>
        {state.imageLoaded
           ? React.null
           : <Text> "Please wait while image is loaded."->React.string </Text>}
      </View>
      <Button title="Reset Image" onPress={_ => dispatch(SetPath(None))} />
    </View>;
};
```

---

## Changelog

Check the [changelog](./CHANGELOG.md) for more informations about recent
releases.

---

## Contribute

Read the
[contribution guidelines](https://github.com/reason-react-native/.github/blob/master/CONTRIBUTING.md)
before contributing.

## Code of Conduct

We want this community to be friendly and respectful to each other. Please read
[our full code of conduct](https://github.com/reason-react-native/.github/blob/master/CODE_OF_CONDUCT.md)
so that you can understand what actions will and will not be tolerated.
