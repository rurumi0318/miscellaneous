# Enhanced JSON Converter for Unreal Engine 5

A simple extension to Unreal's built-in JSON conversion that supports dynamic fields through optional post-processing methods.

## The Problem

Unreal's `FJsonObjectConverter` only handles `UPROPERTY` fields. Some JSON data can't be represented as `UPROPERTY`:

```cpp
USTRUCT()
struct FMyData 
{
    GENERATED_BODY()
    
    UPROPERTY()
    FString Name;  // ✅ Works fine
    
    TSharedPtr<FJsonObject> DynamicData;  // ❌ Can't be UPROPERTY
};
```

## The Solution

Add optional `PostJsonImport` and `PostJsonExport` methods to your structs:

```cpp
USTRUCT()
struct FMyData 
{
    GENERATED_BODY()
    
    UPROPERTY()
    FString Name;
    
    TSharedPtr<FJsonObject> DynamicData;  // Dynamic field
    
    // Handle dynamic fields during JSON import
    void PostJsonImport(const TSharedPtr<FJsonObject>& JsonObject)
    {
        if (JsonObject.IsValid())
        {
            const TSharedPtr<FJsonObject>* DynamicPtr;
            if (JsonObject->TryGetObjectField(TEXT("DynamicData"), DynamicPtr))
            {
                DynamicData = *DynamicPtr;
            }
        }
    }
    
    // Handle dynamic fields during JSON export
    TSharedPtr<FJsonObject> PostJsonExport(TSharedPtr<FJsonObject> JsonObject) const
    {
        if (JsonObject.IsValid() && DynamicData.IsValid())
        {
            JsonObject->SetObjectField(TEXT("DynamicData"), DynamicData);
        }
        return JsonObject;
    }
};
```

## Usage

Replace `FJsonObjectConverter` with `FEnhancedJsonConverter`:

```cpp
// Instead of this:
// FJsonObjectConverter::JsonStringToUStruct(JsonString, MyStruct);

// Use this:
FEnhancedJsonConverter::JsonStringToUStruct(JsonString, MyStruct);
```

## Available Functions

```cpp
// JSON string ↔ UStruct
FEnhancedJsonConverter::JsonStringToUStruct(JsonString, OutStruct);
FEnhancedJsonConverter::UStructToJsonString(InStruct, OutJsonString);

// JSON object ↔ UStruct  
FEnhancedJsonConverter::JsonObjectToUStruct(JsonObject, OutStruct);
FEnhancedJsonConverter::UStructToJsonObject(InStruct);
```

## How It Works

1. **Standard conversion**: Uses Unreal's `FJsonObjectConverter` for all `UPROPERTY` fields
2. **Post-processing**: Automatically calls your `PostJsonImport`/`PostJsonExport` methods if they exist
3. **Compile-time detection**: Uses SFINAE to detect methods at compile time - no runtime overhead if methods don't exist

## Example JSON

```json
{
    "Name": "TestItem",
    "DynamicData": {
        "CustomField": "CustomValue",
        "AnotherField": 42,
        "NestedObject": {
            "InnerData": "InnerValue"
        }
    }
}
```

The `Name` field is handled by standard conversion, while `DynamicData` is processed by your custom methods.

## License

MIT License