type source;

external fromRequired: ReactNative.Packager.required => source = "%identity";
external fromUriSource: string => source = "%identity";

type offset;

[@bs.obj] external offset: (~x: int, ~y: int) => offset;

type size;

[@bs.obj] external size: (~width: int, ~height: int) => size;

type cropData;

[@bs.obj]
external cropData:
  (
    ~offset: offset,
    ~size: size,
    ~displaySize: size=?,
    ~resizeMode: [ | `contain | `cover | `stretch]=?,
    unit
  ) =>
  cropData;

[@bs.module "@react-native-community/image-editor"] [@bs.scope "default"]
external cropImage: (source, cropData) => Js.Promise.t(string) = "cropImage";
