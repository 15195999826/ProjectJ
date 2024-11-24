import unreal
import pypinyin

@unreal.uclass()
class PythonBridgeImplementation(unreal.PythonBridge):

    @unreal.ufunction(override=True)
    def function_implemented_in_python(self):
        unreal.log_warning("Wow! 111")

    @unreal.ufunction(override=True)
    def test(self):
        unreal.log_warning("Wow! Test!!")
        return "Test"

    @unreal.ufunction(override=True)
    def to_pinyin(self, in_text):
        # 将中文字符串转换为拼音
        pinyin_text = pypinyin.lazy_pinyin(in_text)
        # 将拼音列表转换为字符串, 并且每个拼音的首字母大写
        pinyin_string = ''.join([s[0].upper() + s[1:] for s in pinyin_text])
        unreal.log_warning(pinyin_string)
        return pinyin_string